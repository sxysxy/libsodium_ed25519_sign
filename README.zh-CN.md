# libsodium_ed25519_sign

[English README](./README.md)

这是一个从 libsodium 中裁剪出来的最小 Ed25519 签名/验签实现。

## 为什么有这个仓库

这个仓库的目标是只保留“数据签名”和“签名校验”真正需要的最精简代码。

动机很直接：

- 减少依赖面
- 让代码路径只服务于 sign/verify 场景
- 更方便静态链接（避免动态链接注入绕过verify）和代码审查
- 当你只需要 Ed25519 签名校验时，不必再带上整个 libsodium

典型应用场景包括：

- 校验通信内容在传输过程中没有被篡改
- 校验软件 activation key 或 license 数据没有被修改

这个仓库聚焦于“完整性校验”场景，而不是提供一个通用的大而全密码学库。

## 接口

```c
int ed25519_sign(const unsigned char private_key[32],
                 const unsigned char *data,
                 size_t data_len,
                 unsigned char signature[64]);

int ed25519_verify(const unsigned char public_key[32],
                   const unsigned char *data,
                   size_t data_len,
                   const unsigned char signature[64]);
```

## 约定

- `private_key` 是 32 字节 Ed25519 seed
- `public_key` 是 32 字节 Ed25519 公钥
- `signature` 是 64 字节 Ed25519 签名
- `ed25519_verify()` 返回 `1` 表示成功，返回 `0` 表示失败

关于 hex 长度：

- 32 字节的公钥或私钥，通常会写成 64 个 hex 字符
- 这是正常的，因为 2 个 hex 字符表示 1 个字节
- 所以长度为 64 的 hex 字符串，实际仍然表示 32 字节二进制数据

如果你需要把 64 个 hex 字符的公钥或私钥转换成 32 字节缓冲区，可以使用：

```c
int ed25519_util_hex2bytes(unsigned char *out,
                           size_t out_len,
                           const char *hex);
```

## 使用方式

普通分离编译方式：

```c
#include "ed25519_sign.h"
```

编译示例：

```sh
cc -std=c99 -O2 your_file.c ed25519_sign.c -o your_program
```

单头文件方式，请拷贝single-header-only中的内容，然后：

```c
#include "ed25519_sign.h"
```

根据一个整数种子初始化 `rand()`，再生成一对公私钥：

```sh
cc -std=c99 -O2 gen_keypair.c -o gen_keypair
./gen_keypair 12345
```

这个工具会：

- 用你输入的整数初始化 C 标准库 `rand()`
- 生成一个 32 字节 Ed25519 seed
- 输出生成出来的 32 字节私钥和 32 字节公钥

这个方式适合测试和演示，不适合生产环境下的安全密钥生成。

## 测试说明

`test.c` 支持两种模式：

- 默认模式：包含 `ed25519_sign.h`，并链接 `ed25519_sign.c`
- header-only 模式：使用仅头文件模式

交互流程如下：

1. 先输入一条要签名的消息
2. 程序内部对这条消息做签名
3. 再输入一条用于校验的消息
4. 如果第二次输入与第一次完全一致，则校验通过
5. 如果第二次输入不同，则校验失败

运行普通分离编译测试：

```sh
./build_test_separate.sh
```

运行单头文件测试：

```sh
./build_test_header_only.sh
```

这两个脚本会自动输入两次相同内容，所以预期输出应包含：

- `verification passed`

如果你想手动验证“内容被篡改”的场景，可以直接运行生成出来的可执行文件，并在第二次输入时改成不同内容。
