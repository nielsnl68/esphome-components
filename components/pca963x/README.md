# PCA963x output component

This component controles the **PCA963x** family led drivers. At this moment you can choose the

- PCA9633 (4 channel version)
- PCA9634 (8 channel version) **\***
- PCA9635 (16 channel version) **\***

(*) Not yet tested.

you can use it like:

```yaml
pca963x:
  model: pca9633

light:
  - platform: rgb
    name: "LCD backlight"
    red: output_red
    green: output_green
    blue: output_blue

# Example output entry
output:
  - platform: pca963x
    id: output_blue
    channel: 0
  - platform: pca963x
    id: output_green
    channel: 1
  - platform: pca963x
    id: output_red
    channel: 2
```
