#!/bin/sh

# Generate 100 numbers between (0 <= (x) <= 1000)
if [[ $# -eq 0 ]]; then
  n=100
  max=1000
  min=0
# Generate (n) numbers between (0 <= (x) <= 1000)
elif [[ $# -eq 1 ]]; then
  n=$1
  max=1000
  min=0
# Generate (n) numbers between (0 <= (x) <= max)
elif [[ $# -eq 2 ]]; then
  n=$1
  max=$2
  min=0
# Generate (n) numbers between (min <= (x) <= max)
elif [[ $# -eq 3 ]]; then
  n=$1
  max=$2
  min=$3
  if [[ min -lt 0 ]]; then
    min=$[$min * -1]
  fi
fi

for (( i = 1; i <= n; i++ )); do
  printf "%s" $((RANDOM % max - min))
  if [[ i -eq n ]]; then
    printf "\n"
  else
    printf ","
  fi
done
