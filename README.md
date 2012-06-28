libmtpipe
=========

C Multi-threaded pipeline library.

Multi-threaded pipeline
===

Multi-threaded pipeline is a synchronous method of parallelizing sequential jobs on different threads. 

Consider the below pseudo code example:

```pseudo
function network_streaming()
	buf = capture_video_input()
	buf_resized = resize_buffer(buf)
	buf_mpeg = mpeg_encode(buf_resized)
	network_transfer(buf_mpeg)
```

In this case, a network streaming functionality is  implemented by sequential functions. Suppose each of buffer operations is a blocking call, the overall latency shall be the sum of the blocking times.

Actually, this is ineffecient on a certain envirionment where we can exploit multicore CPU, or coprocessor IO, etc.. 

To preserve the original sequential semantics and do parallize the jobs, some pipelining architecure is required.

See the next example:

```pseudo
function network_streaming()
	pipeline(capture => resize => mpeg => transfer)

thread capture()
	buf = capture_video_input()
	fifo_put(buf)

thread resize()
	buf = fifo_get()
	buf_resized = resize_buffer(buf)
	fifo_put(buf_resized)

thread mpeg()
	buf_resized = fifo_get()
	buf_mpeg = mpeg_encode(buf_resized)
	fifo_put(buf_mpeg)

thread transfer()
	buf_mpe = fifo_get()
	network_transfer(buf_mpeg)
```

There we separated each jobs to different threads. In each of `thread` definitions, input and output buffers are implicitly provided/passed to other threads by fifo_xxx features. Note that fifo_put() and fifo_get() are well described by exclusive enqueue/dequeueing of buffer references. 

The connection of threads are described by `pipeline` dummy syntax with the order of data flow in its clause. 

This method is pretty convenient where we can either

 * arrange processing order of pipeline (only if data types are consistent),
 * reuse thread functions,
 * refactor side-effect localities.

**libmtpipe** provides minimal C interfaces to thread-pipelining operations of those characteristics, but more. 

The correspondece between above examples and implementations are:

 * `thread` -> pthread,
 * `fifo_xxx` -> Unix-pipe,
 * `pipeline` -> thread pool and connection DSL.


Build Instruction
=====
* Host

```bash
autoreconf
./configure
make
```

* Cross compile (ARM example)

```bash
autoreconf
./configure --host=arm --cross-prefix=arm_v5t_le-
make
```

If succeeded, lib/libmtpipe.so and lib/libmtpiped.so (debug binary) are created.

License
===

MIT