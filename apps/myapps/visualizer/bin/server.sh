#!/bin/sh

ruby -rwebrick -e 'Thread.start{WEBrick::HTTPServer.new(DocumentRoot:"bin",Port:"7777").start};gets'
chrome "http://localhost:7777/emptyExample.html"
