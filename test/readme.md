# Stress Test
using the sample project
ab -n 10000 -c 100 http://localhost:<PORT>/index.html
ab -n 10000 -c 100 http://localhost:9004/index.html