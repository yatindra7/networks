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
static uint16_t Checksum(void* buf, int len){

    int* buffer = (int*)buf;
    int sum = 0;
    int result;

    // uint16_t* buffer = (uint16_t*)buf;
    // uint32_t sum = 0;
    // uint16_t result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buffer++;

    if (len == 1)
        sum += *(uint8_t*)buffer;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


// not tested
/**
Given an IP address, performs a reverse DNS lookup to find the corresponding hostname.
* @param ip_address const char* - IP address in string format
* @param host_name char* - pointer to a character buffer where the hostname will be stored
* @param host_name_size: int - size of the character buffer
* @return void
*/
void ReverseDNSLookup(const char* ip_address, char* host_name, int host_name_size) {}

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
static int SendICMPPacket(int socket_fd, const char* dest_address, int ttl, int sequence_number, int packet_size) {}


/**
* Given the send and receive times of an ICMP packet, calculates the latency in milliseconds.
* @param send_time: const struct timeval* - pointer to the timeval struct representing the send time
* @param recv_time: const struct timeval* - pointer to the timeval struct representing the receive time
* @return double - latency in milliseconds
*/
static double EstimateLatency(const struct timeval* send_time, const struct timeval* recv_time) {}


/**
* Given the packet size and the latency, estimates the bandwidth in bits per second.
* @param packet_size: int - size of the ICMP packet in bytes
* @param latency: double - latency in milliseconds
* @return: double - estimated bandwidth in bits per second
*/
static double EstimateBandwidth(int packet_size, double latency){}

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
static void PrintPacketDetails(const char* address, const struct sockaddr_in* from, int packet_size, int ttl, double latency, double bandwidth){}

/**
 * Prints the IP header fields.
 *
 * @param ip_header The pointer to the IP header structure.
 */
static void PrintIPHeader(struct iphdr* ip_header){}

/**
 * Prints the TCP header fields.
 *
 * @param tcp_data The pointer to the TCP data.
 */
static void PrintTCPHeader(char* tcp_data){}

/**
 * Prints the UDP header fields.
 *
 * @param udp_data The pointer to the UDP data.
 */
static void PrintUDPHeader(char* udp_data){}

/**
 * Prints a message indicating that the protocol is unknown.
 */
static void PrintUnknownProtocolHeader(){}

/**
 * Performs a traceroute to the specified address, estimates the latency and bandwidth of each
 * link in the path, and returns the information for each node in the path.
 *
 * @param address The IP address or hostname of the target.
 * @param node_count A pointer to an integer that will store the number of nodes in the path.
 * @return An array of NodeInfo structures, containing information about each node in the path.
 *         The number of elements in the array is stored in the integer pointed to by node_count.
 */
NodeInfo* PingNetInfo(const char* address, int* node_count){}

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

    printf("it reaches the end :)\n");
    return 0;
}
