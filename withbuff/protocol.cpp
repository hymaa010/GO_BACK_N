#include "protocol.h"
#include <queue>
#include <bits/stdc++.h>
#include <cstring>
using namespace std;

/* -----------------------------  
   Simulated Buffers  
   ----------------------------- */

queue<packet> network_layer_buffer;   
queue<frame> physical_layer_buffer;   

bool network_layer_enabled = true;

/* -----------------------------  
   Timer Simulation  
   ----------------------------- */

static const int TIMER_LIMIT = 5;
static int frame_timer[100];
static bool frame_timer_running[100];

static int ack_timer = 0;
static bool ack_timer_running = false;

/* -----------------------------  
   Network Layer  
   ----------------------------- */

void network_layer_event(packet p) {
    network_layer_buffer.push(p);
}

int network_layer_event_count() {
    return (int)network_layer_buffer.size();
}

void wait_for_event(event_type *event) {

    *event = cksum_err;

    if (!physical_layer_buffer.empty()) {
        *event = frame_arrival;
        return;
    }

    if (network_layer_enabled && !network_layer_buffer.empty()) {
        *event = network_layer_ready;
        return;
    }

    for (int i = 0; i < 100; i++) {
        if (frame_timer_running[i]) {
            frame_timer[i]--;
            if (frame_timer[i] <= 0) {
                frame_timer_running[i] = false;
                *event = timeout;
                return;
            }
        }
    }

    if (ack_timer_running) {
        ack_timer--;
        if (ack_timer <= 0) {
            ack_timer_running = false;
            *event = ack_timeout;
            return;
        }
    }
}

void from_network_layer(packet *p) {
    if (!network_layer_buffer.empty()) {
        *p = network_layer_buffer.front();
        network_layer_buffer.pop();
    } else {
        memset(p->data, 0, 1);
    }
}

void to_network_layer(packet *p) {
    cout << "[PHYSICAL] Delivered packet with first byte: " 
         << (int)p->data[0] << endl;
}

void enable_network_layer(void) { network_layer_enabled = true; }
void disable_network_layer(void) { network_layer_enabled = false; }

/* -----------------------------  
   Physical Layer  
   ----------------------------- */

/* Drop only the FIRST transmission of frame 2 */
bool to_physical_layer(frame *s) {
    static bool dropped_frame2 = false;
    
    if (s->info.data[0]=='H' && !dropped_frame2) {
        dropped_frame2 = true;
        return true;    // drop this ONE transmission
    }

    physical_layer_buffer.push(*s);
    return false;
}

void from_physical_layer(frame *r) {
    if (!physical_layer_buffer.empty()) {
        *r = physical_layer_buffer.front();
        physical_layer_buffer.pop();
    }
}

/* -----------------------------  
   Timers  
   ----------------------------- */

void start_timer(seq_nr k) {
    frame_timer[k] = TIMER_LIMIT;
    frame_timer_running[k] = true;
}

void stop_timer(seq_nr k) {
    frame_timer_running[k] = false;
}

void start_ack_timer(seq_nr k) {
    ack_timer = TIMER_LIMIT;
    ack_timer_running = true;
}

void stop_ack_timer(seq_nr k) {
    ack_timer_running = false;
}
