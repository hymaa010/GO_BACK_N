#include "protocol.h"
#include <queue>
#include <bits\stdc++.h>
#include <cstring>
using namespace std;

/* -----------------------------
   Simulated Event Variables
   ----------------------------- */

queue<packet> network_layer_buffer;   // packets from application layer
queue<frame> physical_layer_buffer;   // frames arriving from channel

bool network_layer_enabled = true;

/* Timer simulation */
static const int TIMER_LIMIT = 5;
static int frame_timer[100];
static bool frame_timer_running[100];

static int ack_timer = 0;
static bool ack_timer_running = false;

/* -----------------------------
    EVENT GENERATION
   ----------------------------- */

void network_layer_event(packet p) {
    network_layer_buffer.push(p);
}

int network_layer_event_count() {
    return (int)network_layer_buffer.size();
}

void wait_for_event(event_type *event) {
    // 1. If a frame arrives from physical layer
    *event=cksum_err;
    if (!physical_layer_buffer.empty()) {
        *event = frame_arrival;
        return;
    }
    // 2. If the network has a packet ready
    if (network_layer_enabled && !network_layer_buffer.empty()) {
        *event = network_layer_ready;
        return;
    }
    // 3. Check timers
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

/* -----------------------------
    NETWORK LAYER SIMULATION
   ----------------------------- */

void from_network_layer(packet *p) {
    if (!network_layer_buffer.empty()) {
        *p = network_layer_buffer.front();
        network_layer_buffer.pop();
    } else {
        // create dummy packet
        memset(p->data, 0, 1);
    }
}

void to_network_layer(packet *p) {
    cout << "[PHYSICAL] Delivered packet with first byte: " << (int)(p->data[0]) << endl;
}

void enable_network_layer(void) {
    network_layer_enabled = true;
}

void disable_network_layer(void) {
    network_layer_enabled = false;
}

/* -----------------------------
   PHYSICAL LAYER SIMULATION
   ----------------------------- */


bool drop = false;
void set_drop(bool val) {
    drop = val;
}

bool to_physical_layer_drop(frame *s) {
    static set<int> already_dropped;
    if (!already_dropped.count(s->seq)) {
        already_dropped.insert(s->seq);
        return true;
    }
    physical_layer_buffer.push(*s);
    return false;
}

bool to_physical_layer(frame *s) {
    if(drop){
        return to_physical_layer_drop(s);
    }
    static bool already_dropped = false; 

    if (s->seq == 2 && !already_dropped) {
        already_dropped = true;
        return true;
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
          TIMERS
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