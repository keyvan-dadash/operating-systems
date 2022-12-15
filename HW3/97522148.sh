#!/bin/bash


#first function validate
validate() {
    local day=$(date +%A -d "$1 $2 $3")
    # echo ${day,,}
    # echo ${4,,}
    if [[ "${day,,}" != "${4,,}" ]] 
    then
        echo "weekday is wrong"
    else
        echo "weekday is correct"
    fi
}

remain() {
    local y=$(date +%Y)
    local y1=$(date +%Y -d 'now + 1 year')
    # local diffrence=$(( ($(date +%s) - $(date +%s -d "$1 $2 $y")) / (60*60*24) ))

    local prev=$(date +%m/%d/%Y -d 'now - 6 month')
    local date=$(date +%m/%d/%Y -d "$1 $2 $y")
    local date1=$(date +%m/%d/%Y -d "$1 $2 $y1")
    local diffrence
    if [[ $date > $prev ]]
    then
        diffrence=$(( ($(date +%s) - $(date +%s -d $date)) / (60*60*24)  ))
        echo "$diffrence passed from your birthday"
    else
        diffrence=$(( ($(date +%s -d $date1) - $(date +%s)) / (60*60*24) ))
        echo "$diffrence remain to your birthday"
    fi

}


diff() {
    local diffrence=$(( ($(date +%s -d $4) - $(date +%s -d "$1 $2 $3")) / (60*60*24) ))
    echo "diffrence between two date is ${diffrence}"
}


read -p "Enter date(ex:4 october 2002 friday) " day month year weekday #example 4 october 2002 friday

validate $day $month $year $weekday
remain $day $month
diff $day $month $year "10/20/2004"
