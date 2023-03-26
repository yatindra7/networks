#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

// PingNetInfo configuration
#define MAX_HOPS 30 // Maximum number of hops to probe
#define PACKET_SIZE 64 // ICMP packet size (in bytes)
#define TIMEOUT_SEC 1 // Timeout for receiving ICMP response (in seconds)
#define MAX_TRIES 5 // Maximum number of times to send a packet
#define MAX_WAIT_MS 5000 // Maximum wait time (in milliseconds) for response to arrive
#define DELAY_MS 1000 // Delay between packets (in milliseconds)

// ICMP packet types
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0
#define ICMP_TIME_EXCEEDED 11

// Structure to hold information about a network node
typedef struct {
    char ip_address[16]; // IP address (in dotted decimal notation)
    char host_name[256]; // Host name (if available)
    uint16_t sequence_number; // Sequence number for current packet
    double latency; // Estimated latency (in microseconds)
    double bandwidth; // Estimated bandwidth (in Mbps)
} NodeInfo;


// Function prototypes

static int GetSocket(void);
static uint16_t Checksum(void* buf, int len);
static void ReverseDNSLookup(const char* ip_address, char* host_name, int host_name_size);
static int SendICMPPacket(int socket_fd, const char* dest_address, int ttl, int sequence_number, int packet_size);
static double EstimateLatency(const struct timeval* send_time, const struct timeval* recv_time);
static double EstimateBandwidth(int packet_size, double latency);
static void PrintPacketDetails(const char* address, const struct sockaddr_in* from, int packet_size, int ttl, double latency, double bandwidth);
static void PrintIPHeader(struct iphdr* ip_header);
static void PrintTCPHeader(char* tcp_data);
static void PrintUDPHeader(char* udp_data);
static void PrintUnknownProtocolHeader();
NodeInfo* PingNetInfo(const char* address, int* node_count);

/**
 * Creates a raw socket and returns the file descriptor.
 * @return int - file descriptor of the raw socket
*/
static int GetSocket(void){
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }
    return socket_fd;
}

/**
 * Calculates the Internet Checksum of a given buffer of data.
 *
 * @param buffer The buffer of data for which the Internet Checksum is to be
 *               calculated.
 * @param len   The size of the buffer of data.
 * @return       The 16-bit Internet Checksum of the given data.
 */


static uint16_t Checksum(void* buf, int len) {
    uint16_t* buffer = buf;
    uint32_t sum = 0;
    uint16_t result;

    // int* buffer = buf;
    // int sum = 0;
    // int result;

    // Calculate the sum of all 16-bit words
    for (int i = 0; i < (len / 2); i++) {
        sum += ntohs(buffer[i]); // Add the 16-bit word to the sum
    }

    // If the length is odd, add the last byte to the sum
    if (len % 2 == 1) {
        sum += ((uint8_t*)buf)[len - 1];
    }

    // Add the carry to the sum
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    // Take the one's complement of the sum
    result = (uint16_t)(~sum);

    return result;
}

void test_checksum(){
     uint16_t buf[] = {0x4500, 0x003c, 0x1a2b, 0x0000, 0x8001, 0xb227, 0x0a00, 0x0001, 0x0a00, 0x0002};
    int len = sizeof(buf);
    uint16_t checksum = Checksum(buf, len);

    printf("Checksum: 0x%04x\n", checksum);
}


// not tested
/**
Given an IP address, performs a reverse DNS lookup to find the corresponding hostname.
* @param ip_address const char* - IP address in string format
* @param host_name char* - pointer to a character buffer where the hostname will be stored
* @param host_name_size: int - size of the character buffer
* @return void
*/
void ReverseDNSLookup(const char* ip_address, char* host_name, int host_name_size) {
    struct sockaddr_in sa;  
    socklen_t len = sizeof(sa); 
    char ip[INET_ADDRSTRLEN]; // This creates a character array ip with a size of INET_ADDRSTRLEN (which is a constant defined in arpa/inet.h), which will be used to store the IP address in string form

    // Convert IP address from string to binary form and store it in the sockaddr_in struct
    inet_pton(AF_INET, ip_address, &(sa.sin_addr)); // This converts the IP address from string form (ip_address) to binary form, and stores it in the sin_addr field of sa. The AF_INET argument specifies that the IP address is in IPv4 format

    // Use getnameinfo to perform reverse DNS lookup on the IP address and store the result in host_name
    if (getnameinfo((struct sockaddr*)&sa, len, host_name, host_name_size, NULL, 0, 0) != 0) {  // This calls the getnameinfo function to perform the reverse DNS lookup. The first argument is a pointer to a sockaddr struct (which we cast to a sockaddr*), containing the IP address to look up. The second argument is the size of the sockaddr struct (len). The third argument is a pointer to the character array host_name, which will store the result of the lookup. The fourth argument is the size of host_name. The last three arguments are set to 0, indicating that we don't need any additional flags or services. The if statement checks whether getnameinfo returns a non-zero value, indicating that there was an error with the lookup
        strncpy(host_name, ip_address, host_name_size - 1); // This copies the IP address from ip_address to host_name, in case the reverse DNS lookup failed. The host_name_size - 1 argument ensures that we don't overflow the host_name buffer
    }
}

// not tested
/**
* Sends an ICMP packet to the specified destination address with the given TTL and sequence number.
* @param socket_fd: int - file descriptor of the raw socket
* @param dest_address: const char* - destination IP address in string format
* @param ttl: int - TTL value for the ICMP packet
* @param sequence_number: int - sequence number for the ICMP packet
* @param packet_size: int - size of the ICMP packet to be sent
* @return int - number of bytes sent, or -1 if an error occurred
*/
static int SendICMPPacket(int socket_fd, const char* dest_address, int ttl, int sequence_number, int packet_size) {
    // initialize a sockaddr_in struct dest_addr
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(dest_address);

    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.checksum = 0;
    icmp_header.un.echo.id = htons(getpid() & 0xFFFF);
    icmp_header.un.echo.sequence = htons(sequence_number);
    
    char* icmp_packet = (char*) malloc(packet_size);
    memset(icmp_packet, 0, packet_size);
    memcpy(icmp_packet, &icmp_header, sizeof(struct icmphdr));
    memset(icmp_packet + sizeof(struct icmphdr), 'x', packet_size - sizeof(struct icmphdr));
    ((struct icmphdr*) icmp_packet)->checksum = Checksum(icmp_packet, packet_size);
    
    struct timeval send_time;
    gettimeofday(&send_time, NULL);
    
    if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
        fprintf(stderr, "Error setting TTL value: %s\n", strerror(errno));
        return -1;
    }
    
    int bytes_sent = sendto(socket_fd, icmp_packet, packet_size, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (bytes_sent <= 0) {
        fprintf(stderr, "Error sending packet: %s\n", strerror(errno));
        free(icmp_packet);
        return -1;
    }
    
    free(icmp_packet);
    return 0;
}


/**
* Given the send and receive times of an ICMP packet, calculates the latency in milliseconds.
* @param send_time: const struct timeval* - pointer to the timeval struct representing the send time
* @param recv_time: const struct timeval* - pointer to the timeval struct representing the receive time
* @return double - latency in milliseconds
*/
static double EstimateLatency(const struct timeval* send_time, const struct timeval* recv_time) {
    double latency = ((double) (recv_time->tv_sec - send_time->tv_sec)) + ((double) (recv_time->tv_usec - send_time->tv_usec)) / 1000000.0;
    return latency;
}

void test_estimated_latency(){
    
    // Set up test values
    struct timeval send_time = { .tv_sec = 1, .tv_usec = 500000 };
    struct timeval recv_time = { .tv_sec = 2, .tv_usec = 0 };

    // Calculate latency
    double latency = EstimateLatency(&send_time, &recv_time);

    // Print result
    printf("Latency: %f seconds\n", latency);
}


/**
* Given the packet size and the latency, estimates the bandwidth in bits per second.
* @param packet_size: int - size of the ICMP packet in bytes
* @param latency: double - latency in milliseconds
* @return: double - estimated bandwidth in bits per second
*/
static double EstimateBandwidth(int packet_size, double latency);

/**
* Prints the details of the received ICMP packet.
* @param address: const char* - IP address in string format of the intermediate node
* @param from: const struct sockaddr_in* - pointer to the sockaddr_in struct representing the source address of the received packet
* @param packet_size: int - size of the received packet in bytes
* @param ttl: int - TTL value of the received packet
* @param latency: double - latency in milliseconds of the received packet
* @param bandwidth: double - estimated bandwidth in bits per second of the received packet
* @return void
*/
static void PrintPacketDetails(const char* address, const struct sockaddr_in* from, int packet_size, int ttl, double latency, double bandwidth);

/**
 * Prints the IP header fields.
 *
 * @param ip_header The pointer to the IP header structure.
 */
static void PrintIPHeader(struct iphdr* ip_header);

/**
 * Prints the TCP header fields.
 *
 * @param tcp_data The pointer to the TCP data.
 */
static void PrintTCPHeader(char* tcp_data);

/**
 * Prints the UDP header fields.
 *
 * @param udp_data The pointer to the UDP data.
 */
static void PrintUDPHeader(char* udp_data);

/**
 * Prints a message indicating that the protocol is unknown.
 */
static void PrintUnknownProtocolHeader();

/**
 * Performs a traceroute to the specified address, estimates the latency and bandwidth of each
 * link in the path, and returns the information for each node in the path.
 *
 * @param address The IP address or hostname of the target.
 * @param node_count A pointer to an integer that will store the number of nodes in the path.
 * @return An array of NodeInfo structures, containing information about each node in the path.
 *         The number of elements in the array is stored in the integer pointed to by node_count.
 */
NodeInfo* PingNetInfo(const char* address, int* node_count);

/**
 * Main function of the program. It takes a site address as input and finds
 * the route and estimates the latency and bandwidth of each link in the path.
 * It uses a combination of ICMP Echo and ICMP Time Exceeded packets to achieve
 * this. The route discovery and estimation of link latency/bandwidth is done
 * using the Traceroute and Ping algorithms.
 *
 * @return     An integer indicating the exit status of the program.
 */

int main(){
    test_checksum();
    test_estimated_latency();
    return 0;
}
