#!/bin/sh

# Extract the .tar file 
echo "Extracting raw data files..."
cd ../datasets
tar -xvzf ../datasets/datasets.tgz
cd ../scripts

# Paths to raw and parsed data files
raw_data_path=../datasets/smhi-opendata_1_53430_20261007_164857_Lund.csv
parsed_data_path=../datasets/parseddata.csv

# Headers of our parsed data .csv file 
echo "year,month,day,hour,temperature" > $parsed_data_path
echo "Created pareseddata.csv file..." 

# Parse the desired raw data file 
echo "Parsing dataset for Lund..."

# Skip rows with descriptive text
skip_rows=27

# Parse data using awk with delimiter ;
# Only include data that is controlled
awk -F';' '$4 == "G" {

    # Split date into year, month and day and store in 
    # an array called date
    split($1, date, "-")

    # Split time into hour, minute and second and store in 
    # an array called time. Only the hour will be used
    split($2, time, ":")

    # Print formated data to stdout
    printf("%i,%i,%i,%i,%0.1f\n", date[1], date[2], date[3], time[1], $3)
    
}' $raw_data_path >> $parsed_data_path

echo "Done" 


