//
//  MTPipeline.h
//  EyeJang
//
//  Created by Yukishita Yohsuke on 2013/11/27.
//  Copyright (c) 2013年 monadworks. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <mtpipe.h>

#import "Freight.h"

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

- (Freight *)retainFreight:(Freight *)freight;
@end

@interface MTNode : NSObject
@property (nonatomic, readonly) BOOL (^setup)(MTNode *);
@property (nonatomic, readonly) BOOL (^process)(MTNode *);
@property (nonatomic, readonly) void (^teardown)(MTNode *);
@property (nonatomic, readonly) NSUInteger num_out_get;
- (Freight *)inGet;
- (Freight *)outGet;
- (BOOL)inPut:(Freight *)freight;
- (BOOL)outPut:(Freight *)freight;
- (BOOL)jointJob:(BOOL (^)(Freight *src, Freight *dst))block;
- (BOOL)sinkJob:(BOOL (^)(Freight *src))block;

@end