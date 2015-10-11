syntax on
syntax enable
filetype on
colorscheme torte

set showmatch
set number
set hlsearch

set nocp
filetype plugin on
let g:netrw_browse_split = 4
let g:netrw_liststyle = 3
let g:netrw_preview = 1
let g:netrw_winsize = -150

nmap <silent> <F3> :vimgrep <C-R><C-W> **<CR>
nmap <silent> <F7> :cn<CR>
nmap <silent> <F8> :cp<CR>
