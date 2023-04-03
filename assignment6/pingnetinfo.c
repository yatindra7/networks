
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <pthread.h>
# include <poll.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <signal.h>
# include <sys/sem.h>
# include <sys/select.h>
# include <sys/un.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/time.h>

# define SA (struct sockaddr*)
# define RECV_TIMEOUT 1
# define STR10 "ownwegvgrh"
# define STR20 "cgmpaubupiashvvixwuk"
# define STR30 "ixwmupxylgbpicnmsijzapvmhcskgw"
# define STR40 "mrppgfxtnenqqkrozlumbgzrlenjwqrkjgzqnylb"

typedef struct s_traceroute
{
	char *buffer; // stores the message (with all the headers)
	char buff[4146]; // 
    // char *buff;
	socklen_t len; // clilen
	struct sockaddr_in	addr; // stores ip address of final destination (one we get after ip lookup)
	struct sockaddr_in	addr2;
	struct icmphdr *icmphd2; // stores the header of message which is recieved
	char *sbuff; // stores the output of create mssg
	char *ip; // stores ip address of final destination
	int	hop; // stores the current hop number
	int	sockfd; // sockfd or raw socket
	struct timeval tv_out; // stores the timeout value
	struct timeval start; // stores the starting time
	struct timeval end; // stores the time when mssg was recved
	double total; 
	int	i; // ith probe
} t_traceroute;

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con);
unsigned short checksum(char *buffer, int nwords);
void exit_err(char *s);
int	per_hop(t_traceroute *p, int num_probes,int T);
void print_results(int type, t_traceroute *p, int n, int num_probes);
void *create_msg(int hop, char *ip, char *buff,int data_len);

void exit_err(char *s)
{
	printf("%s", s);
	exit(1);
}

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con)
{
	struct addrinfo	hints;
	struct addrinfo	*res;
	struct sockaddr_in *sa_in;
	char *ip;

	memset(&(hints), 0, sizeof(hints));
	hints.ai_family = AF_INET;
	ip = malloc(INET_ADDRSTRLEN);
	if (getaddrinfo(addr_host, NULL, &hints, &(res)) < 0)
		exit_err("ft_traceroute: unknown host\n");
	sa_in = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(res->ai_family, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);
	(*addr_con) = *sa_in;
	(*addr_con).sin_port = htons(1);
	return (ip);
}

unsigned short checksum(char *buffer, int nwords)
{
	unsigned short *buf;
	unsigned long sum;

	buf = (unsigned short *)buffer;
	sum = 0;
	while (nwords > 0)
	{
		sum += *buf++;
		nwords--;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (~sum);
}

int per_hop(t_traceroute *p, int num_probes, int T)
{
    // initialise 2 arrays to store time difference and length of different messages sent 
    // length of message = 0, 10, 20, 30, 40 -- (i*10) %50 

    int * mssg_size;
    mssg_size = (int*)malloc(sizeof(int)*num_probes);
    double * total_time;
    total_time = (double *) malloc(sizeof(double) * num_probes);
    // initialise both of them
    for(int idx = 0; idx < num_probes; idx++){
        mssg_size[idx] = (idx%5)*10;
    }
    for (int idx = 0; idx < num_probes; idx++) {
        total_time[idx] = -1.0;
    }
	while (++(p->i) < num_probes) // number of probes done
	{
		p->sbuff = create_msg(p->hop, p->ip, p->buffer,mssg_size[p->i]); // sbuf stores the buffer
		gettimeofday(&p->start, NULL); // stores sending time at start
		int num;
        num = sendto(p->sockfd, p->sbuff, sizeof(struct ip) + sizeof(struct icmphdr) + mssg_size[p->i], 0, SA & p->addr, sizeof(p->addr));
		if (!(recvfrom(p->sockfd, p->buff, sizeof(p->buff), 0, SA & p->addr2, &p->len) <= 0))
		{
			gettimeofday(&p->end, NULL); // store end time at last
            // calculate the time difference
			p->total = (double)((p->end.tv_usec - p->start.tv_usec) / 1000.0);
            total_time[p->i] = (double)((p->end.tv_usec - p->start.tv_usec) / 1000.0);
            // stores the icmphdr of the packet we recieved
			p->icmphd2 = (struct icmphdr *)(p->buff + sizeof(struct ip));

			if ((p->icmphd2->type != 0))
				print_results(1, p, p->i,num_probes); // print delay
			else
			{
                // printf("here ");
				print_results(1, p, p->i,num_probes);
				if (p->i == num_probes-1) // condition to stop the hops, reached final dest/ not an icmp echo
                   return (1);
			}
		}
		else{
            // printf("type == %d", p->icmphd2->type);
			print_results(2, p, p->i,num_probes); // print * // time over
            if (p->i == num_probes-1 && p->icmphd2->type == 0) // condition to stop the hops, reached final dest/ not an icmp echo
                return (1);
        }
        sleep(T);
	}
    free(total_time);
    free(mssg_size);
	return (0);
}


void *create_msg(int hop, char *ip, char *buff,int data_len)
{
	struct ip *ip_hdr;
	struct icmphdr *icmphd;

	ip_hdr = (struct ip *)buff;
	ip_hdr->ip_hl = 5; // header length
	ip_hdr->ip_v = 4; // version
	ip_hdr->ip_tos = 0; // type of service
	ip_hdr->ip_len = sizeof(struct ip) + sizeof(struct icmphdr) + data_len; // @add change this
	ip_hdr->ip_id = 10000; // id
	ip_hdr->ip_off = 0;
	ip_hdr->ip_ttl = hop; // ttl
	ip_hdr->ip_p = IPPROTO_ICMP; // protocol
	inet_pton(AF_INET, ip, &(ip_hdr->ip_dst)); // destination ip addrress
	ip_hdr->ip_sum = checksum(buff, 9); // checksum
	icmphd = (struct icmphdr *)(buff + sizeof(struct ip)); // creates icmp header
	icmphd->type = ICMP_ECHO; // sets type to icmp echo
	icmphd->code = 0; // sets code to zero
	icmphd->checksum = 0; // 
	icmphd->un.echo.id = 0; // identifier = 0
	icmphd->un.echo.sequence = hop + 1; // sequence number = current hop + 1
	icmphd->checksum = checksum((buff + 20), 4); //checksum of icmp header
    switch(data_len) {
    case 10:
        memcpy(buff + sizeof(struct ip) + sizeof(struct icmphdr), STR10, data_len);
        break;
    case 20:
        memcpy(buff + sizeof(struct ip) + sizeof(struct icmphdr), STR20, data_len);
        break;
    case 30:
        memcpy(buff + sizeof(struct ip) + sizeof(struct icmphdr), STR30, data_len);
        break;
    case 40:
        memcpy(buff + sizeof(struct ip) + sizeof(struct icmphdr), STR40, data_len);
        break;
    // more case statements can be added here
    default:
        // code to be executed if none of the above cases match
        break;
}
	return (buff);
}

void init_trace(t_traceroute *trace)
{
	int	one;
	int	*val;

	one = 1;
	val = &one; 
	trace->hop = 1; // first hop
	trace->tv_out.tv_sec = RECV_TIMEOUT; // set timeout
	trace->tv_out.tv_usec = 0; // microsec of timeout = 0
	trace->len = sizeof(struct sockaddr_in); // length of sockaddr_in 
	trace->buffer = malloc(4146); // have to change i think @add
	trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // creates a socket
	if (setsockopt(trace->sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) // tells kernel to use user given ip packet and not add it's own
		exit_err("error setsockopt\n");
	setsockopt(trace->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&trace->tv_out, sizeof(trace->tv_out)); // sets timeout value
}

void print_results(int type, t_traceroute *p, int n, int num_probes)
{
	struct ip *ip;
	// struct hostent *c;
	char *ipa;

	ipa = inet_ntoa(p->addr2.sin_addr);
	// ip = (struct ip *)p->buff;
	// c = gethostbyaddr((void*)&(ip->ip_src.s_addr), sizeof(ip->ip_src.s_addr), AF_INET);
	if (type == 1)
	{
		if (n == 0)
		{
			printf("%2d  %15s %.3f ms ", p->hop,ipa, p->total);
		}
		else
			printf("%.3f ms%c", p->total, (n == num_probes-1) ? '\n' : ' ');
	}
	else
	{
		if (n == 0)
			printf("%2d      * ", p->hop);
		else
			printf("      *%c", (n == num_probes-1) ? '\n' : ' ');
	}
}

void ft_traceroute(t_traceroute *p, int num_probes, int T)
{
	while ((!(p->hop == 31))) // sets the max hop value to 30
	{
		p->i = -1; // set i to -1 , keeps count of how many packets have been sent till now
		if (per_hop(p,num_probes,T)) // do perhop, when it returns +1 it means it has reached the final destination
			break ;
		p->hop++; // increase the value of ttl
	}
}

int	main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("incomplete arguments\n ");
        exit(-1);
    }

    char *trace_dest = argv[1]; //  a site to probe (can be either a name like cse.iitkgp.ac.in or an IP address like 10.3.45.6)
    int num_probes = atoi(argv[2]); // the number of times a probe will be sent per link
    int T = atoi(argv[3]); // time difference between any two probes
    if(T < 1){
        printf("T should be >= 1");
        exit(-1);
    }
    if(num_probes < 5){
        printf("Num of probes shld be atlest 5\n");
        exit(-1);
    }
	t_traceroute trace; // stores info about the trace

	init_trace(&trace); // initialises values of the trace
	trace.ip = dns_lookup(trace_dest, &trace.addr); // dns lookup
	// @add another condition to check if ip is given already
    if (trace.ip) 
	{
		printf("traceroute to %s (%s), 30 hops max\n", trace_dest, trace.ip);
		ft_traceroute(&trace, num_probes,T); // do traceroute
		free(trace.buffer); // free memory // need to change
	}
	return (0);
}
