//パーティクルの数
RWByteAddressBuffer particleCountBuffer : register(u1);
//パーティクルのindexデータ
RWByteAddressBuffer particleIndexBuffer : register(u3);
RWByteAddressBuffer newIndexBuffer : register(u4);
RWStructuredBuffer<uint> deleteIndexBuffer : register(u5);
