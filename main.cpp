#include <bits/stdc++.h>
#include "protocol.h"
using namespace std;
#define MAX_SEQ 7

bool between(seq_nr a, seq_nr b, seq_nr c)
{
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return (true);
    else
        return (false);
}

void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
    frame s;
    s.info = buffer[frame_nr];
    s.seq = frame_nr;
    s.ack = (frame_expected) % (MAX_SEQ + 1);
    cout<< "[PHYSICAL] Sent frame " << s.seq << endl;
    if(to_physical_layer(&s)) cout << "\n[PHYSICAL] *** SIMULATED LOSS: Dropping first transmission of frame "
             << s.seq << " ('" << s.info.data[0] << "') ***\n" << endl;
    start_timer(frame_nr);
    
}

void protocol5(void)
{
    seq_nr next_frame_to_send, ack_expected, frame_expected;
    seq_nr nbuffered, i;
    event_type event;
    frame r;
    packet buffer[MAX_SEQ + 1];
    int count=network_layer_event_count();
    enable_network_layer();
    ack_expected = 0;
    next_frame_to_send = 0;
    frame_expected = 0;
    nbuffered = 0;
    bool flaged=0;
    while (true)
    {
        bool lost=1; 
        wait_for_event(&event);
        switch (event)
        {
        case network_layer_ready:
            from_network_layer(&buffer[next_frame_to_send]);
            nbuffered++;
            cout << "[NETWORK] Sent frame " << (next_frame_to_send) % (MAX_SEQ + 1) << " with first byte: " 
            << (int)(buffer[(next_frame_to_send) % (MAX_SEQ + 1)].data[0]) << endl;
            send_data(next_frame_to_send, frame_expected, buffer);
            inc(next_frame_to_send);
            
            break;
        case frame_arrival:
            from_physical_layer(&r);
            if (r.seq == frame_expected )
            {
                lost=0;
                to_network_layer(&r.info);
                r.ack = (frame_expected) % (MAX_SEQ + 1);
                inc(frame_expected);
                cout << "[NETWORK] Received frame " << r.seq << "\n[NETWORK] Sent ACK " << r.ack << endl;
                count--;
                if (!count) return ;
            }

            while (between(ack_expected, r.ack, next_frame_to_send) && !lost)
            {
                nbuffered--;
                stop_timer(ack_expected);
                inc(ack_expected);
            }
            break;
        case cksum_err:
            break;
        case timeout:
            next_frame_to_send = ack_expected;
            for (i = 1; i <= nbuffered; i++)
            {
                send_data(next_frame_to_send, frame_expected, buffer);
                inc(next_frame_to_send);
            }
        }
        if (nbuffered < MAX_SEQ)
            enable_network_layer();
        else
            disable_network_layer();
        cout<< "----------------------------------" << endl;
    }
}

void intialize()
{
    
    cout << "\n----------------------------------------\n";
    cout << "   STARTING PROTOCOL 5 SIMULATION\n";
    cout << "----------------------------------------\n\n";
    
    // ───────────────────────────────────────────────
    // SCENARIO 1
    // ───────────────────────────────────────────────
    cout << ">>> SCENARIO 1: Initial Batch (A-E)\n";
    cout << "----------------------------------------\n";
    for(unsigned char c : {'A','B','C','D','E'})
    network_layer_event({c});
    protocol5();    
    
    // ───────────────────────────────────────────────
    // SCENARIO 2
    // ───────────────────────────────────────────────
    cout << "\n>>> SCENARIO 2: Middle Batch (F-I)\n";
    cout << "----------------------------------------\n";
    for(unsigned char c : {'F','G','H','I'})
    network_layer_event({c});
    protocol5();
    
    // ───────────────────────────────────────────────
    // SCENARIO 3
    // ───────────────────────────────────────────────
    set_drop(true);
    cout << "\n>>> SCENARIO 3: Final Batch (J-L)\n";
    cout << "----------------------------------------\n";
    for(unsigned char c : {'J','K','L'})
        network_layer_event({c});
    protocol5();
}

int main() 
{
    cout << "========================================\n";
    cout << "      GO_BACK_N PROTOCOL 5\n";
    cout << "========================================\n\n";
    cout << "\n========================================\n";
    cout << "          FIRST ASSUMPTION\n";
    cout << "========================================\n";
    intialize();
    cout << "\n========================================\n";
    cout << "        PROTOCOL SIMULATION ENDED\n";
    cout << "========================================\n";

    return 0;
}