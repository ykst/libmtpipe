//
//  MTPipeline.h
//  EyeJang
//
//  Created by Yukishita Yohsuke on 2013/11/27.
//  Copyright (c) 2013年 monadworks. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef COCOAPOD
#include "mtpipe.h"
#else
#include <mtpipe.h>
#endif

@class MTNode;

@interface MTPipeline : NSObject

+ (MTPipeline *)createPipeline;

- (MTNode *)createNodeWithSetup:(BOOL (^)(MTNode *))setup withProcess:(BOOL (^)(MTNode *))process withTeardown:(void (^)(MTNode *))teardown;


- (MTNode *)createConduit;

- (BOOL)start;
- (void)pause;
- (void)chain:(MTNode *)source to:(MTNode *)target;
- (void)bind:(MTNode *)source to:(MTNode *)target;
- (void)joint:(MTNode *)source to:(MTNode *)target;
- (void)extend:(MTNode *)source to:(MTNode *)target;
- (void)converge;
- (void)killall;

- (id)retainFreight:(id)freight;
@end

@interface MTNode : NSObject
@property (nonatomic, copy) BOOL (^setup)(MTNode *);
@property (nonatomic, copy) BOOL (^process)(MTNode *);
@property (nonatomic, copy) void (^teardown)(MTNode *);
@property (nonatomic, readonly) NSUInteger num_out_get;
@property (nonatomic, readonly) NSUInteger num_out_put;
@property (nonatomic, readonly) NSUInteger num_in_get;
@property (nonatomic, readonly) NSUInteger num_in_put;
- (BOOL)feedInGet:(id)freight;
- (BOOL)feedOutGet:(id)freight;
- (id)inGet;
- (id)outGet;
- (BOOL)isPaused;
- (BOOL)inPut:(id)freight;
- (BOOL)outPut:(id)freight;
- (BOOL)jointJob:(BOOL (^)(id src, id dst))block;
- (BOOL)sinkJob:(BOOL (^)(id src))block;
- (BOOL)sourceJob:(BOOL (^)(id))block;

@end
