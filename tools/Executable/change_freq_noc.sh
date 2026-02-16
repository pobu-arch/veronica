target=2500000
for d in /sys/devices/system/cpu/intel_uncore_frequency/uncore*; do
  echo $target > $d/min_freq_khz
  echo $target > $d/max_freq_khz
done

# verify
for d in /sys/devices/system/cpu/intel_uncore_frequency/uncore*; do
  echo -n "$d current: " && cat $d/current_freq_khz
done
