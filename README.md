# osdev

run.bat --> For compile and run quickly

src :

-tests asm -> Few function using BIOS until the activation of protected mode.  
    ----printInputStringWithStack -> save an user's input in the stack and print it  
    ----printIntegers -> print the content of registers  
    ----readDisk -> like his name :)  
    ----protectedMode -> GDT Table and enter to protected mode  
    ----VGAHelloWorld -> Hello World ! in protected mode.  
    
-kernel Hello World v1 -> basic C functions for printing with VGA text mode.  
  ----boot.asm -> bootloader  
  ----kernEntry -> call main from kernel.c  
  ----zeroes.asm -> fill sectors with empty bytes  
  ----kernel.c -> like his name :)  
  ----inoutlib(.c/.h) -> library to manage output (Soon input) stream  

More soon :D  


  Credits :  
  
  I learned the ASM part thanks to this playlist : https://www.youtube.com/playlist?list=PLm3B56ql_akNcvH8vvJRYOc7TbYhRs19M (I was stuck a lot of time but thanks to his explanation I got through it <3)  
  My inspiration for start my learning : https://youtu.be/ELTwwTsR5w8?si=5TPEGrpiS7oGRkry  
 
