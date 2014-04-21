//
//  MTPipeline.m
//  EyeJang
//
//  Created by Yukishita Yohsuke on 2013/11/27.
//  Copyright (c) 2013年 monadworks. All rights reserved.
//

#import "MTPipeline.h"

@interface MTNode()
@property (nonatomic, readwrite) mtnode_handle mtnode;
@end

@implementation MTNode

- (BOOL)feedInGet:(id)freight
{
    if (!mtnode_feed_inget(self.mtnode, (__bridge void *)freight)) {
        return NO;
    }
    return YES;
}

- (BOOL)feedOutGet:(id)freight
{
    if (!mtnode_feed_outget(self.mtnode, (__bridge void *)freight)) {
        return NO;
    }
    return YES;
}

- (id)inGet
{
    void *rowp = nil;
    if (!mtnode_inget(self.mtnode, &rowp)) {
        return nil;
    }
    return (__bridge id)rowp;
}

- (id)outGet
{
    void *rowp = nil;
    if (!mtnode_outget(self.mtnode, &rowp)) {
        return nil;
    }
    return (__bridge id)rowp;
}

- (BOOL)inPut:(id)freight
{
    if (!mtnode_input(self.mtnode, (__bridge void *)freight)) {
        return NO;
    }
    return YES;
}

- (BOOL)outPut:(id)freight
{
    if (!mtnode_output(self.mtnode, (__bridge void *)freight)) {
        return NO;
    }
    return YES;
}

- (NSUInteger)num_out_get
{
    return mtnode_num_outget(self.mtnode);
}

- (BOOL)isPaused
{
    return mtnode_is_pause(self.mtnode);
}

- (BOOL)jointJob:(BOOL (^)(id , id))block
{
    id src = (id)[self inGet];
    id dst = (id)[self outGet];

    if (!src || !dst) return NO;

    if (!block(src, dst)) return NO;

    if (![self outPut:dst]) return NO;
    if (![self inPut:src]) return NO;

    return YES;
}

- (BOOL)sinkJob:(BOOL (^)(id))block
{
    id f = (id)[self inGet];

    if (!f) return NO;

    if (block) {
        if (!block(f)) return NO;
    }

    if (![self inPut:f]) return NO;

    return YES;
}

@end

@interface MTPipeline() {
    @protected
    NSMutableArray *_nodes; // just for retain
    NSMutableArray *_freights;
    mtpipe_handle _handle;
}
@end

@implementation MTPipeline

static void *__node_loop(void *arg)
{
    mtnode_handle node = (mtnode_handle)arg;
    MTNode *bridged_self = (__bridge MTNode *)(mtnode_priv(node));
    BOOL setup_success = YES;

    @autoreleasepool {
        if (bridged_self.setup) {
            setup_success = bridged_self.setup(bridged_self);
        }
    }

    mtpipe_sync_init(node);

    if (setup_success ) {
        while (!mtnode_is_done(node)) {
            @autoreleasepool {
                mtnode_pause_wait(node);
                if (!bridged_self.process(bridged_self)) {
                    mtnode_done(node);
                    break;
                }
            }
        }
    }

    mtpipe_sync_finish(node);

    @autoreleasepool {
        if (bridged_self.teardown) {
            bridged_self.teardown(bridged_self);
        }
    }

    return MTPIPE_THREAD_SUCCESS;
}

+ (MTPipeline *)createPipeline
{
    MTPipeline *obj = [[MTPipeline alloc] init];
    return obj;
}


- (MTNode *)createNodeWithSetup:(BOOL (^)(MTNode *))setup withProcess:(BOOL (^)(MTNode *))process withTeardown:(void (^)(MTNode *))teardown
{
    MTNode *obj = [MTNode new];
    obj.setup = setup;
    obj.process = process;
    obj.teardown = teardown;

    [self _addNode:obj];

    return obj;
}

- (MTNode *)createConduit
{
    MTNode *obj = [MTNode new];

    obj.mtnode = mtnode_create_conduit(_handle);
    [_nodes addObject:obj]; // retain reference

    return obj;
}

- (id)init
{
    self = [super init];
    if (self) {
        _handle = mtpipe_create(NULL);
        _nodes = [NSMutableArray array];
        _freights = [NSMutableArray array];
    }
    return self;
}

- (void)_addNode:(MTNode *)node
{
    node.mtnode = mtnode_create(_handle, object_getClassName(node), __node_loop, 0, (__bridge void *)node);

    [_nodes addObject:node]; // retain reference
}

- (void)dealloc
{
    if (_handle) {
        mtpipe_delete(_handle);
        _handle = nil;
    }
}

- (BOOL)start
{
    return mtpipe_start(_handle);
}

- (void)converge
{
    (void)mtpipe_converge(_handle);
}

- (void)pause
{
    mtpipe_pause_on(_handle);
}

- (void)chain:(MTNode *)source to:(MTNode *)target
{
    (void)mtpipe_chain(source.mtnode, target.mtnode);
}

- (void)joint:(MTNode *)source to:(MTNode *)target
{
    (void)mtpipe_joint(source.mtnode, target.mtnode);
}

- (void)bind:(MTNode *)source to:(MTNode *)target
{
    (void)mtpipe_bind(source.mtnode, target.mtnode);
}

- (void)extend:(MTNode *)source to:(MTNode *)target
{
    (void)mtpipe_extend(source.mtnode, target.mtnode);
}

- (void)killall
{
    if (_handle) {
        if (!mtpipe_is_done(_handle)) {
            mtpipe_done(_handle);
            (void)mtpipe_converge(_handle);
        }
        mtpipe_delete(_handle);
        _handle = nil;
    }
}

- (id)retainFreight:(id)freight
{
    [_freights addObject:freight];
    return freight;
}

@end

