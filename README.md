# 🚀 Go-Back-N without buffer vs Go-Back-N with buffer — A Visual & Practical Networking Demo

### *Two C++ implementations that **dramatically** show how ARQ strategies behave when a frame is lost.*

---

## 🏷️ Badges

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Stars](https://img.shields.io/github/stars/yourname/yourrepo?style=social)

---

## 🎯 Why This Repository Exists

I built **two complete ARQ protocol simulations** — one classic **Go-Back-N (Protocol 5)** and one **Selective Repeat-style (Protocol 6-like)** — to answer a simple question:

> **“What *actually* happens on the wire when a single frame is lost?”**

Both versions intentionally **drop the very first transmission of the frame containing the letter `'H'`**, causing a dramatic divergence in behavior:

* Go-Back-N retransmits **the entire window** 🤯
* Selective Repeat retransmits **only the missing frame** 😎

With detailed `[NETWORK]` and `[PHYSICAL]` logs, the contrast becomes crystal clear.

---

## 📟 Terminal Output Example — Go-Back-N (`withoutbuff/`)

When the `'H'` frame is lost, everything after it must be resent:

```text
[SENDER] Sending frame seq=3 data='H'
[PHYSICAL] *** DROPPED frame seq=3 ***
[NETWORK] Timeout! Resending window...
[SENDER] Resending seq=3
[SENDER] Resending seq=4
[SENDER] Resending seq=5
...
[RECEIVER] Out-of-order frame discarded (expected seq=3)
```

---

## 📟 Terminal Output Example — Selective Repeat (`withbuff/`)

Selective Repeat only retransmits the missing `'H'` frame and buffers the rest:

```text
[SENDER] Sending frame seq=3 data='H'
[PHYSICAL] *** DROPPED frame seq=3 ***
[RECEIVER] Received out-of-order seq=4 → buffered
[RECEIVER] Missing frame seq=3 → NACK sent
[SENDER] Retransmitting seq=3 due to NACK
[RECEIVER] Delivered seq=3,4,5 in order 🎉
```

---

## 📁 Repository Structure

```
withbuff/              → Selective Repeat ARQ (receiver buffering + NACK simulation)
  main.cpp
  protocol.cpp
  protocol.h

withoutbuff/           → Classic Go-Back-N ARQ (no receiver buffering)
  main.cpp
  protocol.cpp
  protocol.h

animation.py           (optional visualization script)
README.md
```

---

## ⚔️ Comparison: Go-Back-N vs Selective Repeat-Style

| Feature                    | Go-Back-N (`withoutbuff`) | Selective Repeat (`withbuff`)      |
| -------------------------- | ------------------------- | ---------------------------------- |
| **Sender Window**          | Size = MAX_SEQ            | Size = MAX_SEQ                     |
| **Receiver Window**        | 1 (only next expected)    | MAX_SEQ (buffers allowed)          |
| **Out-of-order Frames**    | ❌ Discarded               | ✅ Buffered in `received[]`         |
| **Retransmission on Loss** | Entire window             | Only missing frames                |
| **NACK Support**           | ❌ No                      | ✅ Explicit NACKs for gaps          |
| **Bandwidth Efficiency**   | 🔴 Low when loss occurs   | 🟢 High, minimal waste             |
| **Protocol Style**         | Protocol 5 (GBN)          | Protocol 6-like (Selective Repeat) |

---

## 🛠️ How to Compile & Run

Both versions are simple single-file C++ builds.

### Go-Back-N (withoutbuff/)

```bash
cd withoutbuff
g++ -std=c++17 main.cpp protocol.cpp -o gbn
./gbn
```

### Selective Repeat-style (withbuff/)

```bash
cd withbuff
g++ -std=c++17 main.cpp protocol.cpp -o sr
./sr
```

---

## 📘 Detailed Behavior Explanation

### 🔵 Go-Back-N (Protocol 5)

* Sender maintains a window of outstanding frames
* Receiver accepts **only the next in-order frame**
* Any out-of-order frame is **discarded**
* If *any* frame in the window is lost, the sender must **retransmit the entire window**
* No selective acknowledgments or NACKs
* Simple but inefficient under non-ideal conditions

**In this repo:**
Dropping `'H'` forces the sender to resend **3–5 frames**, even though only one was lost.

---

### 🟢 Selective Repeat (Protocol 6-like)

* Sender and receiver both maintain sliding windows
* Receiver **buffers out-of-order frames** in a `received[]` array
* Missing frames generate **NACKs**
* Only lost frames are retransmitted
* Much higher channel utilization
* Slightly more complex because the receiver must reorder buffered frames

**In this repo:**
Frames `4` and `5` are safely buffered even while `'H'` (seq=3) is missing. After the NACK, only seq=3 is resent.

---

## ✨ What I Learned

* How small implementation differences lead to **huge performance differences** in ARQ protocols
* Why Selective Repeat is dramatically more efficient when the channel is lossy
* How to design clean sender/receiver logging (**[NETWORK]**, **[PHYSICAL]**) for protocol visualization
* How buffering logic works internally (received[] window, NACK triggers, gap detection)
* Why textbooks show diagrams, but **code reveals the truth**

---

## 🎯 Perfect For

* 📚 Networking course assignments
* 🧠 Interview prep for systems/network engineering
* 🧑‍🏫 Teaching ARQ protocols interactively
* 🗂️ Portfolio projects showing real protocol insight
* 🛠️ Anyone who wants to *see* ARQ mechanisms instead of just reading about them

---

## 📚 References

* Andrew S. Tanenbaum — *Computer Networks*
* Kurose & Ross — *Computer Networking: A Top-Down Approach*

---

## 😊 Thanks for Visiting!

If you enjoyed this project, **please ⭐ the repo** — it really helps!
Contributions, improvements, and animations are always welcome.
