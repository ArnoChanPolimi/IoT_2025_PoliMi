#!/bin/bash

input_pcap="challenge2.pcapng"
output_csv="result.csv"

echo "Request Type,Request Code,Request MID,Source IP,Destination IP,Response Type,Response Code" > "$output_csv"

tshark -r "$input_pcap" -T fields \
  -e coap.type \
  -e coap.code \
  -e coap.mid \
  -e ip.src \
  -e ip.dst \
  -Y 'coap && (ip.src == 127.0.0.1 || ip.dst == 127.0.0.1 || ipv6.src == ::1 || ipv6.dst == ::1)' | \
while IFS=$'\t' read -r type code mid src dst; do
    if [[ "$type" == "0" && "$code" == "3" ]]; then  # Filter PUT requests
        response=$(tshark -r "$input_pcap" -T fields \
          -e coap.type \
          -e coap.code \
          -e coap.mid \
          -e ip.src \
          -e ip.dst \
          -Y "coap && coap.mid == $mid && coap.code >= 128 && (ip.src == 127.0.0.1 || ip.dst == 127.0.0.1 || ipv6.src == ::1 || ipv6.dst == ::1)")

        if [[ -n "$response" ]]; then
            response_type=$(echo "$response" | cut -f1)
            response_code=$(echo "$response" | cut -f2)
            response_src=$(echo "$response" | cut -f3)
            response_dst=$(echo "$response" | cut -f4)

            echo "$type,$code,$mid,$src,$dst,$response_type,$response_code" >> "$output_csv"
        fi
    fi
done

echo "Processing completed. Results saved to $output_csv"

