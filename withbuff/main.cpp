#include <bits/stdc++.h>
#include "protocol.h"
using namespace std;
#define MAX_SEQ 7

bool nack_flag=0,ack_flag=0,error_flag=0;
bool received[MAX_SEQ + 1] = {0};

bool between(seq_nr a, seq_nr b, seq_nr c)
{
    if (((a <= b) && (b < c)) ||
        ((c < a) && (a <= b)) ||
        ((b < c) && (c < a)))
        return true;
    return false;
}

void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
    frame s;
    s.info = buffer[frame_nr];
    s.seq = frame_nr;
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);

    if (!received[s.seq])cout << "[PHYSICAL] Sent frame " << s.seq << endl;

    if (to_physical_layer(&s)){
        nack_flag=1;
        ack_flag=1;
        error_flag=1;
        cout << "\n[PHYSICAL] *** SIMULATED LOSS: Dropping first transmission of frame "
        << s.seq << " ('" << s.info.data[0] << "') ***\n" << endl;
    }
    start_timer(frame_nr);
}

void protocol5(void)
{
    seq_nr next_frame_to_send, ack_expected, frame_expected;
    seq_nr nbuffered, i;
    event_type event;
    frame r;
    packet buffer[MAX_SEQ + 1];
    nack_flag=0;ack_flag=0;
    received[MAX_SEQ + 1] = {0};
    packet recv_buffer[MAX_SEQ + 1];
    int count = network_layer_event_count();
    enable_network_layer();
    ack_expected = 0;
    next_frame_to_send = 0;
    frame_expected = 0;
    nbuffered = 0;

    while (true)
    {
        wait_for_event(&event);
        switch (event)
        {
        case network_layer_ready:
            from_network_layer(&buffer[next_frame_to_send]);

            cout << "[NETWORK] Sent frame " << next_frame_to_send
                 << " with first byte: "
                 << (int)buffer[next_frame_to_send].data[0] << endl;

            send_data(next_frame_to_send, frame_expected, buffer);
            nbuffered++;
            inc(next_frame_to_send);
            break;
        case frame_arrival:
            from_physical_layer(&r);
            if (r.seq == frame_expected)
            {
                to_network_layer(&r.info);
                r.ack = (frame_expected) % (MAX_SEQ + 1);
                cout << "[NETWORK] Received frame " << r.seq <<endl;
                if(!ack_flag) {
                    cout << "[NETWORK] Sent ACK " << r.ack << endl;
                    ack_flag=0;
                }
                count--;
                received[r.seq] = false;
                inc(frame_expected);              
                while (received[frame_expected])
                {
                    count--;
                    to_network_layer(&recv_buffer[frame_expected]);
                    cout << "[NETWORK] Delivered buffered frame " << frame_expected << endl;
                    received[frame_expected] = false;
                    inc(frame_expected);
                }
                if (!count) {
                    if(error_flag)cout << "[NETWORK] Sent ACK " << (frame_expected + MAX_SEQ) % (MAX_SEQ + 1) << endl;
                    return;
                }
            }
            else if (between(frame_expected, r.seq, (frame_expected + MAX_SEQ)))
            {
                if (!received[r.seq])
                {
                    recv_buffer[r.seq] = r.info;
                    received[r.seq] = true;
                }
                cout << "[NETWORK] Buffered out-of-order frame " << r.seq << endl;
                if(nack_flag){
                    nack_flag=0;
                    cout << "[NETWORK] Sent NACK "
                     << (frame_expected + MAX_SEQ) % (MAX_SEQ + 1)+1 << endl;
                }
            }
            while (between(ack_expected, r.ack, next_frame_to_send))
            {
                nbuffered--;
                stop_timer(ack_expected);
                inc(ack_expected);
            }
            break;
        case timeout:
            next_frame_to_send = ack_expected;
            for (i = 1; i <= nbuffered; i++)
            {
                send_data(next_frame_to_send, frame_expected, buffer);
                inc(next_frame_to_send);
            }
            break;
        case cksum_err:
        case ack_timeout:
            break;
        }
        if (nbuffered < MAX_SEQ)
            enable_network_layer();
        else
            disable_network_layer();

        cout << "----------------------------------" << endl;
    }
}

void intialize()
{
    cout << "\n----------------------------------------\n";
    cout << "   STARTING PROTOCOL 5 SIMULATION\n";
    cout << "----------------------------------------\n\n";

    cout << ">>> SCENARIO 1: Initial Batch (A-E)\n";
    cout << "----------------------------------------\n";
    for (unsigned char c : {'A','B','C','D','E'})
        network_layer_event({c});
    protocol5();
    cout << "\n\n>>> SCENARIO 2: Initial Batch (F-J)\n";
    cout << "----------------------------------------\n";
    for (unsigned char c : {'F','G','H','I','J'})
        network_layer_event({c});
    protocol5();
}

int main()
{
    cout << "========================================\n";
    cout << "      GO_BACK_N PROTOCOL 5\n";
    cout << "========================================\n\n";

    cout << "========================================\n";
    cout << "        SECOND ASSUMPTION\n";
    cout << "========================================\n";

    intialize();

    cout << "\n========================================\n";
    cout << "        PROTOCOL SIMULATION ENDED\n";
    cout << "========================================\n";
    return 0;
}
