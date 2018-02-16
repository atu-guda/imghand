#!/bin/bash
dstt=$( date '+%Y%m%d' )
proj='imghead'
git archive --format=tar.gz --prefix=${proj}-${dstt}/ -o ../${proj}-${dstt}.tar.gz HEAD


