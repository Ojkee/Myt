# TODOs

## Essentials

- [x] Lexing cells
- [x] Parsing cells
- [x] Evaluate cells
- [x] Error handling
- [x] Few builtins
- [x] Connect to ui
    - [x] Working interpreter 
    - [x] Scrolling should update value positions
    - [x] Cells interactions and updates
        - [x] Dependencies
        - [x] Cycle detection

## Extras
- [x] Enter and Tab keys selects to down and to right cell respectively
- [x] Refactor reapeated algorithms to Utlis
- [x] Navigation using arrows/enter/tab
- [ ] Saving/Loading sheet
- [ ] Drag&Drop csv files
- [ ] Auto resize number of rows and cols 
- [ ] Changing colors 
- [x] Resize window
- [ ] Move cells around
- [ ] Lambda (??)

```
Lambda(x y => [Exression with 1 output]) 

Apply([Lambda Cell], [Range if multiple params/Cell])
```

## Bugs
- [x] First selections doesn't unselect cells in UI
- [x] Navigating around doesn't update cell value positions 
- [ ] Navigating after offsetting to the right back to the left 
      with set up cell on the left to the selected cell copies value 
- [ ] Too big number limit / string arithmetics
