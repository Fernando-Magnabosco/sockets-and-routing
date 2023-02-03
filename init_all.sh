#!/bin/bash
for id in {1..5}
do
  gnome-terminal -- ./router $id
done
