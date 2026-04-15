These were done with the equations:

```zig
switch (dirt) {
    .normal => 0.0029 * go_stones + 0.372,
    .dirty => 0.0045 * go_stones + 0.1799,
    .nasty => 0.0058 * go_stones + 0.0783,
};
```
