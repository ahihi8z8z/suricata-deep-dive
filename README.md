# generata gsm-map pcap with diffirent imsi
First, using wireshark to export packet to c array and save to file sample.h.
Then locate imsi index in array and set to varriable imsi_idx in sample.c
Then recomplie program.
```
gcc sample.c -o sample
```
First run sample execute file to generate hexdump file, use -h option to see help. 
Example, thist command will create 100000 diffirent imsi gsm-map packet with mcc 452, mnc 04 and write to hexdump file sample.txt.
```
./sample -c 45204 -f sample.txt -n 100000 
```
Then use text2pcap to convert hexdump file to pcap.
```
text2pcap sample.txt sample.pcap
```
