# Valid

```
just text
---
eval: String(just text)
```

```
5 + 5
---
eval: String(5 + 5)
```

```
"just string"
---
eval: String(just  string)
```

```
= 5 * 2
---
eval: Int(10)
```

```
= true
---
eval: Bool(true)
```

```
= C2 * 3
---
eval: Int(value of 'C2' * 3)
```

```
= (7 + 5) / 2
---
eval: Int(6)
```


# Errors

