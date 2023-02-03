#!/bin/bash
for id in {1..6}
do
  gnome-terminal -- ./router $id
done
