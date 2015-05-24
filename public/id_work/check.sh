#!/bin/bash
ls log.*|xargs -i cat {}|awk '{if(!s[$0]){s[$0]=1}else{print $0}}'|wc -l
