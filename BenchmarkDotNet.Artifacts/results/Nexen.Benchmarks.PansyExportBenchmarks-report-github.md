```

BenchmarkDotNet v0.15.8, Windows 10 (10.0.19045.6466/22H2/2022Update)
Intel Core i7-8700K CPU 3.70GHz (Coffee Lake), 1 CPU, 12 logical and 6 physical cores
.NET SDK 10.0.200-preview.0.26103.119
  [Host] : .NET 10.0.3 (10.0.3, 10.0.326.7603), X64 RyuJIT x86-64-v3
  Dry    : .NET 10.0.3 (10.0.3, 10.0.326.7603), X64 RyuJIT x86-64-v3

Job=Dry  IterationCount=1  LaunchCount=1  
RunStrategy=ColdStart  UnrollFactor=1  WarmupCount=1  

```
| Method                         | Mean     | Error | Ratio | Allocated | Alloc Ratio |
|------------------------------- |---------:|------:|------:|----------:|------------:|
| FullExport_Optimized_Medium    | 17.01 ms |    NA |  0.78 |   1.21 MB |        0.84 |
| FullExport_Original_Medium     | 21.67 ms |    NA |  1.00 |   1.44 MB |        1.00 |
| FullExport_WithCpuState_Medium | 22.82 ms |    NA |  1.05 |   5.75 MB |        3.99 |
