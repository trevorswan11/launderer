These were done with the equations:

```zig
switch (dirt) {
    .normal => 0.0017 * go_stones + 0.0398,
    .dirty => 0.0034 * go_stones + 0.0795,
    .nasty => 0.0051 * go_stones + 0.1193,
};
```
