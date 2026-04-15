These were done with bucketing!

```zig
const scalar: f32 = if (go_stones > medium_upper_bound)
    large_scalar
else if (go_stones < medium_lower_bound)
    small_scalar
else
    1.0;

duration_s = switch (dirt) {
    .normal => medium_normal * scalar,
    .dirty => medium_dirty * scalar,
    .nasty => medium_nasty * scalar,
};
```

with the following constants:
```zig
const medium_upper_bound = 420.0;
const medium_lower_bound = 160.0;

const large_scalar = 5.0 / 3.0;
const small_scalar = 1.0 / 3.0;

const medium_normal: f32 = 0.5;
const medium_dirty: f32 = 1.0;
const medium_nasty: f32 = 1.5;
```
