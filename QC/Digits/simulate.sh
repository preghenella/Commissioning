#!/bin/bash

reset

o2-sim -n 10000 -g pythia8 -m TOF |& tee sim.log

