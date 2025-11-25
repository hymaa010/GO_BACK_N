#define MAX_PKT 1024                    /* packet size in bytes */

typedef unsigned int seq_nr;            /* sequence or ACK numbers */

typedef struct {
    unsigned char data[MAX_PKT];
} packet;                               /* packet definition */
typedef enum { data, ack, nak } frame_kind; /* frame kind definition */

typedef enum { 
	network_layer_ready,
	frame_arrival, 
	cksum_err, 
	timeout, 
	ack_timeout 
} event_type;
typedef struct {
    frame_kind kind;                    /* what kind of frame? */
    seq_nr seq;                         /* sequence number */
    seq_nr ack;                         /* ACK number */
    packet info;                        /* the Network layer packet */
} frame;

/* wait for an event to happen; return its type of event */
void wait_for_event(event_type *event);

/* fetch a packet from the network layer for transmission */
void from_network_layer(packet *p);

/* deliver information from an inbound frame to the network layer */
void network_layer_event(packet p);
int network_layer_event_count();
void set_drop(bool val);
void to_network_layer(packet *p);

/* get an inbound frame from the physical layer */
void from_physical_layer(frame *r);

/* pass the frame to the physical layer */
bool to_physical_layer(frame *s);

/* start the clock and enable the timeout event */
void start_timer(seq_nr k);

/* stop the clock and disable the timeout event */
void stop_timer(seq_nr k);

/* start an auxiliary timer and enable the ack_timeout event */
void start_ack_timer(seq_nr k);

/* stop an auxiliary timer and disable the ack_timeout event */
void stop_ack_timer(seq_nr k);

/* allow the network to cause a network_layer_ready event */
void enable_network_layer(void);

/* forbid the network to cause a network_layer_ready event */
void disable_network_layer(void);

/* macro inc */
#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0
