import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches
import numpy as np

# --- CONFIGURATION ---
events = [
    ('send', 0, 'A', 'success'),
    ('ack',  1, '',  'success'),
    ('send', 1, 'B', 'success'),
    ('ack',  2, '',  'success'),
    # SCENARIO: DROP C
    ('send', 2, 'C', 'lost'),    
    ('send', 3, 'D', 'discard'), 
    ('send', 4, 'E', 'discard'),
    # TIMEOUT EVENT
    ('timeout', -1, '', 'timer'),
    # RETRANSMISSION
    ('send', 2, 'C', 'success'),
    ('ack',  3, '',  'success'),
    ('send', 3, 'D', 'success'),
    ('ack',  4, '',  'success'),
    ('send', 4, 'E', 'success'),
    ('ack',  5, '',  'success'),
]

# Setup Figure
fig, ax = plt.subplots(figsize=(9, 10), facecolor='#f0f0f0')
ax.set_xlim(0, 10)
ax.set_ylim(0, 12)
ax.axis('off')

# --- STATIC VISUALS ---

# 1. The Channel (Pipe)
ax.plot([5, 5], [2.5, 9.5], color='#cccccc', linestyle='--', linewidth=2, zorder=0)
ax.text(5.2, 6, "Transmission Channel", color='#999999', fontsize=8, rotation=-90, va='center')

# 2. Sender Node (Top)
sender_box = patches.FancyBboxPatch((2, 9.5), 6, 1.5, boxstyle="round,pad=0.1", 
                                    fc='#e3f2fd', ec='#2196f3', linewidth=2)
ax.add_patch(sender_box)
ax.text(5, 10.25, "SENDER (Network Layer)", ha='center', va='center', fontsize=12, fontweight='bold', color='#0d47a1')

# 3. Receiver Node (Bottom)
receiver_box = patches.FancyBboxPatch((2, 1), 6, 1.5, boxstyle="round,pad=0.1", 
                                      fc='#e8f5e9', ec='#4caf50', linewidth=2)
ax.add_patch(receiver_box)
ax.text(5, 1.75, "RECEIVER (Physical Layer)", ha='center', va='center', fontsize=12, fontweight='bold', color='#1b5e20')

# 4. Console Box (Status)
console_bg = patches.Rectangle((0, 0), 10, 0.8, fc='#212121')
ax.add_patch(console_bg)
status_text = ax.text(0.5, 0.4, "System Ready...", color='#00ff00', fontfamily='monospace', fontsize=11, ha='left', va='center')

# --- DYNAMIC ELEMENTS ---

# The moving packet (CIRCLE)
packet_circle = patches.Circle((5, 9.5), 0.5, fc='#1976d2', ec='white', linewidth=1, alpha=0)
ax.add_patch(packet_circle)

# The text inside the packet
packet_text = ax.text(5, 9.5, "", color='white', ha='center', va='center', fontweight='bold', zorder=5)

# Cross mark for rejected packets
cross_text = ax.text(5, 5, "X", color='red', fontsize=30, fontweight='bold', ha='center', va='center', alpha=0)

# Timeout alert
timeout_text = ax.text(5, 6, "!!! TIMEOUT !!!", color='red', fontsize=20, fontweight='bold', 
                       ha='center', va='center', alpha=0)

# Animation State
current_event_idx = 0
frame_steps = 0
max_steps = 50 # Smooth animation

def init():
    packet_circle.set_alpha(0)
    packet_text.set_text("")
    cross_text.set_alpha(0)
    timeout_text.set_alpha(0)
    return packet_circle, packet_text, status_text, cross_text, timeout_text

def update(frame):
    global current_event_idx, frame_steps
    
    if current_event_idx >= len(events):
        status_text.set_text("> SIMULATION COMPLETED.")
        packet_circle.set_alpha(0)
        packet_text.set_text("")
        return packet_circle, packet_text, status_text, cross_text, timeout_text

    e_type, seq, data, result = events[current_event_idx]
    progress = frame_steps / max_steps

    # Reset transient visuals
    cross_text.set_alpha(0)
    timeout_text.set_alpha(0)
    
    # --- LOGIC ---
    
    if e_type == 'send':
        # Move from Sender (y=9.5) to Receiver (y=2.5)
        start_y = 9.5
        end_y = 2.5
        current_y = start_y - (start_y - end_y) * progress
        
        # Update Circle Position
        packet_circle.center = (5, current_y)
        packet_circle.set_alpha(1)
        
        # Update Text Position
        packet_text.set_position((5, current_y))
        packet_text.set_text(f"{data}\n#{seq}")
        
        # Colors
        if result == 'lost':
            packet_circle.set_facecolor('#d32f2f') # Red
            status_text.set_text(f"> Sending Frame {seq} ('{data}')... LOSS OCCURRING")
            if progress > 0.6:
                new_alpha = 1 - (progress - 0.6) * 3
                packet_circle.set_alpha(max(0, new_alpha))
                packet_text.set_alpha(max(0, new_alpha))
            else:
                packet_text.set_alpha(1)
                
        elif result == 'discard':
            packet_circle.set_facecolor('#ff9800') # Orange
            status_text.set_text(f"> Sending Frame {seq}... REJECTED (Out of Order)")
            if progress > 0.9:
                cross_text.set_position((5, current_y))
                cross_text.set_alpha(1)
        else:
            packet_circle.set_facecolor('#1976d2') # Blue
            status_text.set_text(f"> Sending Frame {seq} ('{data}')...")

    elif e_type == 'ack':
        # Move from Receiver (y=2.5) to Sender (y=9.5)
        start_y = 2.5
        end_y = 9.5
        current_y = start_y + (end_y - start_y) * progress
        
        packet_circle.center = (5, current_y)
        packet_circle.set_facecolor('#388e3c') # Green for ACK
        packet_circle.set_alpha(1)
        packet_text.set_alpha(1)
        
        packet_text.set_position((5, current_y))
        packet_text.set_text(f"ACK\n{seq-1}")
        status_text.set_text(f"> Sending ACK for {seq-1}")

    elif e_type == 'timeout':
        packet_circle.set_alpha(0)
        packet_text.set_text("")
        # Flash logic
        if (frame_steps // 5) % 2 == 0:
            timeout_text.set_alpha(1)
        else:
            timeout_text.set_alpha(0)
        status_text.set_text("> TIMER EXPIRED. Resending...")

    # Step increment
    frame_steps += 1
    if frame_steps >= max_steps:
        frame_steps = 0
        current_event_idx += 1
        
    return packet_circle, packet_text, status_text, cross_text, timeout_text

ani = animation.FuncAnimation(fig, update, frames=range(len(events)*max_steps + 100), 
                              init_func=init, blit=True, interval=30)

plt.title("Go-Back-N Protocol: Loss & Recovery Scenario", fontsize=14, pad=20)
plt.show()
