#!/bin/bash

# Input and output file paths
input_pcap="challenge2.pcapng"
output_csv="CQ2_result.csv"

# Target IP address
target_ip="134.102.218.18"

# Check if the pcap file exists
if [ ! -f "$input_pcap" ]; then
    echo "Error: File $input_pcap does not exist!"
    exit 1
fi

# Initialize temporary files for storage
confirmable_file=$(mktemp)
non_confirmable_file=$(mktemp)
temp_file=$(mktemp)

# Output file header
echo "Resource,Confirmable_GET_Count,Non_Confirmable_GET_Count" > "$output_csv"

# Extract Confirmable GET requests (coap.type == 0 and coap.code == 1) for the target IP
echo "Extracting Confirmable GET requests..."
tshark -r "$input_pcap" -Y "ip.dst == $target_ip && coap.type == 0 && coap.code == 1" -T fields -e coap.opt.uri_path | sort | uniq -c > "$confirmable_file"

# Extract Non-Confirmable GET requests (coap.type == 1 and coap.code == 1) for the target IP
echo "Extracting Non-Confirmable GET requests..."
tshark -r "$input_pcap" -Y "ip.dst == $target_ip && coap.type == 1 && coap.code == 1" -T fields -e coap.opt.uri_path | sort | uniq -c > "$non_confirmable_file"

# Merge the statistics data from both requests and output to a temporary file
echo "Merging statistics data..."
awk '
    NR==FNR{a[$2]=$1; next} 
    {if ($2 in a) print $2, a[$2], $1}
' "$confirmable_file" "$non_confirmable_file" > "$temp_file"

# Filter resources with matching Confirmable and Non-Confirmable GET counts greater than zero
echo "Filtering matching resources..."
awk '$2 == $3 && $2 > 0 {print $1 "," $2 "," $3}' "$temp_file" >> "$output_csv"

# Clean up temporary files
rm "$temp_file" "$confirmable_file" "$non_confirmable_file"

echo "Completed! Results saved to $output_csv"

