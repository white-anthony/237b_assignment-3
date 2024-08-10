CC = gcc 
CFLAGS = -g -Wall

ifeq ($(shell uname -s),Darwin)
    LDFLAGS = -framework OpenCL
else
    LDFLAGS = -L/usr/local/cuda/lib64 -lOpenCL
endif

solution: template.c readMatrix.c
	$(CC) $(CFLAGS) template.c readMatrix.c -o solution $(INCFLAGS) $(LDFLAGS)

run: 
	./solution Dataset/0/input0.raw Dataset/0/input1.raw Dataset/0/output.raw
	./solution Dataset/1/input0.raw Dataset/1/input1.raw Dataset/1/output.raw
	./solution Dataset/2/input0.raw Dataset/2/input1.raw Dataset/2/output.raw
	./solution Dataset/3/input0.raw Dataset/3/input1.raw Dataset/3/output.raw
	./solution Dataset/4/input0.raw Dataset/4/input1.raw Dataset/4/output.raw
	./solution Dataset/5/input0.raw Dataset/5/input1.raw Dataset/5/output.raw
	./solution Dataset/6/input0.raw Dataset/6/input1.raw Dataset/6/output.raw
	./solution Dataset/7/input0.raw Dataset/7/input1.raw Dataset/7/output.raw
	./solution Dataset/8/input0.raw Dataset/8/input1.raw Dataset/8/output.raw
