#!/bin/sh
perltidy -gnu -l=72 -b -bext=~ tests/t/*.t tests/t/d/*.t tests/lib/Test/*.pm
