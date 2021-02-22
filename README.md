# factorOracle
C source for the *factorOracle* Pure Data external.

Currently, the external is verified to run on Mac OS Mavericks 10.9.5 or greater with Pure Data v0.47 through Pure Data v0.51-4.

Visit [https://puredata.info/docs/developer](https://puredata.info/docs/developer) for instructions on how to build the *factorOracle* external for your architecture.

### What does it do?
Factor oracle is a graph representing at least all of the substrings of a word. It can be built incrementally in linear time and space. A factor oracle representation of input from a live musical performance can be built in real time and parsed using a variety of heuristics to generate music in the style of the performance. 

See [factorOracle.pdf](factorOracle.pdf) for a detailed description of this implementation.

See [factorOracle-demo.pd](factorOracle-demo.pd) for usage.

See [https://vimeo.com/adamjameswilson/eighteen](https://vimeo.com/adamjameswilson/eighteen) for a video example of *factorOracle* used in a live performance. 

### License and copyright notice
 
*factorOracle*, a Pure Data external  
Adam James Wilson  
awilson@citytech.cuny.edu  
 
LICENSE:
 
This software is copyrighted by Adam James Wilson and others. The following terms (the "Standard Improved BSD License") apply:
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
