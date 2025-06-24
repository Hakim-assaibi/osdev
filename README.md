# osdev

run.bat --> Quickly compile and run

src/ :

-tests asm -> Few function using BIOS, up to protected mode.  
    ----printInputStringWithStack -> Saves user input on the stack and prints it  
    ----printIntegers -> Prints the content of the registers  
    ----readDisk -> Just like the name says :)  
    ----protectedMode -> Sets up the GDT Table and enters protected mode  
    ----VGAHelloWorld -> Hello World ! in protected mode.  
    
-kernel Hello World v1 -> Basic C functions for printing using VGA text mode.  
  ----boot.asm -> Bootloader  
  ----kernEntry -> Calls main from kernel.c  
  ----zeroes.asm -> Fills sectors with empty bytes  
  ----kernel.c -> The name speaks for itself :)  
  ----inoutlib(.c/.h) -> Library to manage output (Soon input) streams  

More coming soon :D  


  Credits :  
  
  I learned the ASM part thanks to this playlist : https://www.youtube.com/playlist?list=PLm3B56ql_akNcvH8vvJRYOc7TbYhRs19M (I got stuck many times but his explanation helped me through it <3)
  My first inspiration to start learning low-level programming : Terry A.Davis
  My second inspiration to start learning low-level programming is this video : https://youtu.be/ELTwwTsR5w8?si=5TPEGrpiS7oGRkry  
 
