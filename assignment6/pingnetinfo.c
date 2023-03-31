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

#include <ifaddrs.h>
#include <netinet/tcp.h>

#define MAX_TTL 30         // sanity
#define SOCKET_TIMEOUT 10  // in seconds
#define RECV_BUF_LEN 10000
#define PORT 20000
// Structure to hold information about a network node
typedef struct {
    char ip_address[16]; // IP address (in dotted decimal notation)
    char host_name[256]; // Host name (if available)
    uint16_t sequence_number; // Sequence number for current packet
    double latency; // Estimated latency (in microseconds)
    double bandwidth; // Estimated bandwidth (in Mbps)
} NodeInfo;


// Function prototypes

static uint16_t Checksum(void* buf, int len); // tested
struct sockaddr_in* getSrcAddr(); // tested
struct in_addr * DNS_lookup(char* URL); // tested
static double EstimateLatency(const struct timeval* send_time, const struct timeval* recv_time); // tested
static double EstimateBandwidth(int* packet_sizes, double* latencies, int num_packets); // not sure about the formula
NodeInfo* PingNetInfo(const char* address, int* node_count);


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


/**
 * Finds IP address of current interface
 *
 * @return  src_addr
 */
struct sockaddr_in* getSrcAddr() {
    struct ifaddrs *id, *id_tmp;
    struct sockaddr_in *src_addr = NULL;
    int getAddress = getifaddrs(&id);
    if (getAddress == -1)
    {
        perror("Unable to retrieve IP address of interface");
        exit(-1);
    }

    id_tmp = id;
    while (id_tmp) 
    {
        if ((id_tmp->ifa_addr) && (id_tmp->ifa_addr->sa_family == AF_INET))
        {
            src_addr = (struct sockaddr_in *) id_tmp->ifa_addr;
            break;
        }
        id_tmp = id_tmp->ifa_next;
    }

    if (src_addr == NULL) {
        perror("Unable to find IP address of interface");
        exit(-1);
    }

    return src_addr;
}

/**
 * Finds out IP address of the given URL
 *
 * @param URL  -- URL (destination to be traced)
 * @return  IP address of the given URL
 */
struct in_addr * DNS_lookup(char* URL){
    struct hostent *host;
        host = gethostbyname(URL);
        if (host == NULL)
        {
            perror("Invalid host or failed DNS resolution");
            exit(-1);
        }
    return (struct in_addr *)host->h_addr;
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


// not so sure about this one
/**
* Given the packet size and the latency, estimates the bandwidth in bits per second.
* @param packet_size: int* - size of the ICMP packet in bytes
* @param latency: double* - latency in milliseconds
* @return: double - estimated bandwidth in bits per second
*/
static double EstimateBandwidth(int* packet_sizes, double* latencies, int num_packets) {
    int n = num_packets;
    double total_data = 0;
    double total_time = 0;
    for (int i = 0; i < n; i++) {
        total_data += packet_sizes[i];
        total_time += latencies[i];
    }
    double avg_data = total_data / n;
    double avg_time = total_time / n;
    return (avg_data / avg_time) * 8 / 1000000; // Convert to Mbps
}

void test_estimated_bandwidth(){
    int packet_sizes[] = {1000, 2000, 500, 1500};
    double latencies[] = {0.05, 0.1, 0.02, 0.15};
    int n = sizeof(packet_sizes) / sizeof(packet_sizes[0]);
    double bandwidth = EstimateBandwidth( packet_sizes, latencies,n);
    printf("Estimated bandwidth: %.2f Mbps\n", bandwidth);
}


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

int main(int argc, char *argv[]){

    if (argc < 4)
    {
        printf("incomplete arguments\n ");
        exit(-1);
    }

    char *trace_dest = argv[1]; //  a site to probe (can be either a name like cse.iitkgp.ac.in or an IP address like 10.3.45.6)
    int num_probes = atoi(argv[2]); // the number of times a probe will be sent per link
    int T = atoi(argv[3]); // time difference between any two probes

    struct sockaddr_in *src_addr = getSrcAddr();
    printf("PingNetInfo from host: %s\n", inet_ntoa(src_addr->sin_addr));

    struct sockaddr_in dest_addr; 
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    int is_ip_addr = inet_aton(trace_dest, &(dest_addr.sin_addr)); // returns zero if trace dest is invalid
    if (!is_ip_addr)
    {
        dest_addr.sin_addr = *((struct in_addr *)DNS_lookup(trace_dest));
        printf("[DNS resolution] %s resolved to %s\n",trace_dest,inet_ntoa(dest_addr.sin_addr));
    }

    
    // create socket to send tcp messages
    int sendSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (sendSocket < 0){perror("Error creating tcp socket");exit(-1);}

    // create socket to receive icmp messages
    int recvSocket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recvSocket < 0){perror("Error creating icmp socket");exit(-1);}

    char recvBuffer[RECV_BUF_LEN];
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(struct sockaddr_in);
    long numBytesReceived;

    printf("--------------- PingNetInfo results ---------------\n");
    printf("0: %s [start]\n", inet_ntoa(src_addr->sin_addr));
    int i = 0;
    while (i < MAX_TTL)
    {
        i++;

        // set TTL in IP header
        setsockopt(sendSocket, IPPROTO_IP, IP_TTL, &i, sizeof(i));

        errno = 0;
        connect(sendSocket, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));

        int icmpErrorReceived = 0;

        // TTL expired
        if (errno == EHOSTUNREACH)
        {
            while (!icmpErrorReceived)
            {
                // do in loop
                numBytesReceived = recvfrom(recvSocket,recvBuffer,RECV_BUF_LEN,0,(struct sockaddr *)&cli_addr,&cli_len);

                // extract IP header
                struct ip *ip_hdr = (struct ip *)recvBuffer;

                // extract ICMP header
                int ipHeaderLength = 4 * ip_hdr->ip_hl;
                struct icmp *icmp_hdr = (struct icmp *)( (char*) ip_hdr + ipHeaderLength );

                int icmpMessageType = icmp_hdr->icmp_type;
                int icmpMessageCode = icmp_hdr->icmp_code;

                // TTL exceeded
                if (icmpMessageType == ICMP_TIME_EXCEEDED && icmpMessageCode == ICMP_NET_UNREACH)
                {
                    // check if ICMP messages are related to TCP packets
                    struct ip *inner_ip_hdr = (struct ip *)( (char*) icmp_hdr + ICMP_MINLEN);
                    if (inner_ip_hdr->ip_p == IPPROTO_TCP)
                    {icmpErrorReceived = 1;}
                }

                // port unreachable
                else if (icmpMessageType == ICMP_DEST_UNREACH && icmpMessageCode == ICMP_PORT_UNREACH)
                {
                    printf("%d: %s [complete]\n", i, inet_ntoa(dest_addr.sin_addr));
                    printf("--------------- PingNetInfo terminated ---------------\n");
                    exit(0);
                }
            }
            printf("%d: %s\n", i, inet_ntoa(cli_addr.sin_addr));

            // success full so send it 5 tines

            // case: timeout
        } else if ( errno == ETIMEDOUT      // socket timeout
                || errno == EINPROGRESS // operation in progress
                || errno == EALREADY    // consecutive timeouts
                )
        {printf("%d: * * * * * [timeout]\n", i);}

        // case: destination reached
        else if (errno == ECONNRESET || errno == ECONNREFUSED)
        {
            printf("%d: %s [complete]\n",i, inet_ntoa(dest_addr.sin_addr));
            printf("--------------- PingNetInfo terminated ---------------\n");
            exit(0);
        }
        else
        {
            printf("Unknown error: %d sending packet\n", errno);
            exit(-1);
        }
    }
    printf("Unable to reach host within TTL of %d\n", MAX_TTL); 
    printf("--------------- PingNetInfo terminated ---------------\n");
    return -1;
    test_checksum();
    test_estimated_latency();
    test_estimated_bandwidth();
    return 0;
}
