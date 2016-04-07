;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OCTOPACMAN
; IL TOURNE SUR LUI-MEME POUR
; AVALER LE PLUS DE BIJOUX POSSIBLE
; AFIN DE RALLONGER SA LIGNE DE VIE.
; PAR CONTRE SI UN BIJOU LE FRAPPE
; EN DEHORS DE LA BOUCHE LA LIGNE DE
; VIE RACCOURCI.
; REF: http://www.jdroid.ch/index.php?
; inhalt_links=navigation.inc.php&
; inhalt_mitte=
; pixelgames/hexaPac.inc.php
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   * diamant      15 points
;   * Gros losange 10 points
;   * petit losange 5 points
;   * petit carre   1 point
;;;;;;;;;;;;;;;;;;;;;;;;;;
; constantes utiles
;;;;;;;;;;;;;;;;;;;;;;;;;;
EQU STRUC_SIZE 12 ;dim. struct bijou
EQU JEWEL_ROW  8 ; jewel rows
; identifiant bijou
EQU BIG_DIAMOND 3 ; gros diamant
EQU DIAMOND     2 ; diamant
EQU RUBY        1 ; rubis
EQU PERL        0 ; perle
; valeur bijou
EQU BD_VAL 15 ; gros diamant
EQU D_VAL  10 ; petit diamant
EQU R_VAL   5 ; rubis
EQU P_VAL   1 ; perle
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; assignation de registres
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DEF ANGLE  VE ; angle d'octopacman
DEF LLEN   VD ; pointage
DEF COUNT  VC ; nbre bijou visibles
DEF MAXJWL VB ; nbre max. de bijou
DEF SPEED  VA ; vitesse du jeux
DEF CYCLES V9 ; compte cycles jeux

  JP start
credits:
  ASCII "Octo pacman\n"
  ASCII "Jacques Deschenes\n"
  ASCII "Copyright 2015" 
  DB 26

start:  
  CALL show_credits
play_again:
  CLS
; initialize variables
  LD ANGLE, 0 ;orientation pacman
  LD LLEN, 8 ;init ligne de vie
  LD COUNT, 0 ;nbre bijoux affichs
  LD MAXJWL, 4 ;nbre max. de bijoux
  LD SPEED, 20 ;vitesse du jeux 5 lent
  LD CYCLES, 0 
  CALL free_jtable
  CALL draw_octopacman
  CALL update_line

;;;;;;;;;;;;;;;;;;;;    
; boucle du jeux
;;;;;;;;;;;;;;;;;;;;    
game_loop:
  ADD CYCLES, 1
  LD V0,127
  AND V0, CYCLES
  SNE V0,0
  call inc_maxjewel
  PUSH ANGLE
  CALL read_joystick
  POP V0
  SNE V0, ANGLE
  JP game_lp1f
;efface octopacman
  CALL draw_octopacman
  LD V0, ANGLE
;affiche octopacman  
  CALL draw_octopacman
game_lp1f:
  CALL move_jewel
  SNE LLEN, 0
  JP game_over
  SNE COUNT, MAXJWL
;ajout nouveau bijou
;a intervalle 8 cycles
  JP speed_delay
  LD V0, 7
  AND V0, CYCLES
  SNE V0, 0
  CALL new_jewel
speed_delay:    
  LD V0, SPEED
  CALL delay
  JP game_loop

;;;;;;;;;;;;;;;;;;;
; partie terminee
;;;;;;;;;;;;;;;;;;;
game_over:
  LD V0,8
  SCD V0
  SCU V0
  LD V0, 0
  SCRY V1
  SHR V1
  SHR V1
  SHR V1
  PUSH V1
  ADD V1, -8
  SCUR V0,V1
  LD I, MSG_OVER
  PRT
  POP V3
  LD V4, 30
  SCUR V3,V4
  PRN CYCLES
  CALL wait_button
  JP play_again

;;;;;;;;;;;;;;;;;;;;;
; mise a jour de la
; barre de vie
;;;;;;;;;;;;;;;;;;;;;
update_line:
  LD V0,2
  SCD V0
  SCU V0
  SNE LLEN, 0
  RET
  SET FG, 15
  PUSH I
  LD I, REG_SAVE
  LD [I],V2
  LD V2,LLEN
  LD V0, 0
  SCRY V1
  ADD V1,-2
  SETP V0,V1
  ADD V2,-1
  SNE V2, 0
  JP .+3
  ADD V0,1
  JP .-5
  ADD V1,1
  LD V0,0
  LD V2,LLEN
  SETP V0,V1
  ADD V2,-1
  SNE V2,0
  JP .+3
  ADD V1,1
  JP .-5
  LD V2,[I]
  POP I
  RET
    
;;;;;;;;;;;;;;;;;
; lecture du joystick
; et modification de 
; l'angle
; utilise V0, V1
; modifie ANGLE
;;;;;;;;;;;;;;;;;;; 
read_joystick:
  LD V0, 0
  LD V1, LEFT
  SKNP V1
  JP rot_left
  LD V1, RIGHT
  SKNP V1
  JP rot_right
  LD V1, UP
  SKNP V1
  JP rot_2left
  LD V1, DOWN
  SKNP V1
  JP rot_2right
  RET
rot_2left:
  ADD V0, 1
rot_left:
  ADD V0, 1 
  JP rotation
rot_2right:
  ADD V0, -1
rot_right:
  ADD V0, -1
rotation:
  ADD ANGLE, V0
  LD V0, 7
  AND ANGLE, V0
  RET

;;;;;;;;;;;;;;;;;;;;;
; attend que le 
; joueur presse le
; bouton du joystick
;;;;;;;;;;;;;;;;;;;;;   
wait_button:
  LD V0, FIRE_BTN
  SKP V0
  JP .-1
  LD V1, 4
  LD ST, V1
  SKNP V0
  JP .-1
  RET

;;;;;;;;;;;;;;;;;;
; affiche message
; d'information
; du jeu
; utilise V0-V2
;;;;;;;;;;;;;;;;;;
show_credits:
  CLS
  LD I, credits
  LD V0, 0
  SCRY V1
  SHR V1
  LD V2, V1
  SHR V1
  SHR V1
  ADD V1, -8
  SCUR V0,V1
  PRT
  PRT
  PRT
cred_loop:
  LD V0, 2
  CALL delay
  LD V1, 1
  SCU V1
  ADD V2, -1
  SNE V2, 0
  JP cred_exit
  LD V1, FIRE_BTN
  SKP V1
  JP cred_loop
cred_exit:
  LD V1, FIRE_BTN
  SKNP V1
  JP .-1
  RET

;;;;;;;;;;;;;;;;;;;;
; pause
; en tournant dans une 
; boucle vide jusqu' 
; expiration de DT
; argument V0=duree msec
;;;;;;;;;;;;;;;;;;;;    
delay:
  LD DT, V0
  LD V0, DT
  SE V0, 0
  JP .-2
  RET

;;;;;;;;;;;;;;;;;;;;;;
; dessine pacman
; argument: V0=angle
; utilise V0, V1,V2,V3,I
;;;;;;;;;;;;;;;;;;;;;;  
draw_octopacman:
  SCRX V1
  SHR V1
  add V1,-8
  SCRY V2
  SHR V2
  add V2,-8
  LD V3, 64
  LD I, PACM0
draw_xp1b:
  SNE V0, 0
  JP draw_xp1f 
  ADD I, V3
  ADD V0, -1
  JP draw_xp1b
draw_xp1f:  
  DRW V1,V2, 0  
  RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; dessine un bijou 
; argument: V0=index tableau
; utilise V0-V3 et I
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
draw_jewel:
  LD I, JTABLE
  LD V3, STRUC_SIZE
  SNE V0, 0
  JP draw_j1f
  ADD I, V3
  ADD V0, -1
  SE V0, 0
  JP .-3
draw_j1f:   
  LD V3, [I]
  LD I, JEWEL0
  LD V0, JEWEL_ROW
  SHL V0 ;size 8x8pixels
  SHL V0 ;bytes=XSIZE/2*JEWEL_ROW
  SNE V1, 0
  JP .+5
  ADD I, V0
  ADD V1, -1
  SE V1, 0
  JP .-3
  DRW V2,V3, 8
  RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ajoute bijou dans JTABLE
; si il y a une position libre
; utilise V0-V5, I
;;;;;;;;;;;;;;;;;;;;;;;;;;; 
new_jewel:
  RND V4, 3 ; index table
  PUSH V4
  LD V3, STRUC_SIZE
  LD I, JTABLE
  SNE V4, 0
  JP nj_1f
  ADD I, V3
  ADD V4, -1
  JP .-4
nj_1f:
  POP V4    
  LD V5, 0
try_next:   
  LD V1,[I]
  SNE V0, 0
  JP free_slot
  ADD I, V3
  ADD V4, 1
  SE V4, 8 ;boucle debut table
  JP .+3
  LD I, JTABLE
  LD V4, 0
  ADD V5, 1
  SNE V5, 8
  RET
  JP try_next
free_slot:
  RND V1, 2 ;nouveau bijou
  SNE V4, 0
  JP E
  SNE V4, 1
  JP NE
  SNE V4, 2
  JP N
  SNE V4, 3
  JP NW
  SNE V4, 4
  JP WEST
  SNE V4, 5
  JP SW
  SNE V4, 6
  JP S
SE: ; sud est
  SCRX V2
  SCRY V3
  JP store_it
S: ; sud
  SCRX V2
  SHR V2
  ADD V2,-4
  SCRY V3
  JP store_it
SW: ; sud-ouet
  LD V2, 0
  SCRY V3
  ADD V3,-4
  JP store_it
WEST:  ; ouest
  LD V2, 0
  SCRY V3
  ADD V3,-4  
  JP store_it
NW: ; nord-ouest
  LD V2, 0
  LD V3, 0 
  JP store_it
N: ; nord
  SCRX V2
  SHR V2
  ADD V2,-4
  LD V3, 0
  JP store_it
NE: ; nord-est
  SCRX V2
  LD V3, 0
  JP store_it
E: ; est
  SCRX V2
  SCRY V3
  SHR V3
  ADD V3,-4
store_it:   
  LD V0, 1  ; occupee
  LD [I], V3
  LD V0, V4
  CALL draw_jewel
  ADD COUNT, 1
  RET
;;;;;;;;;;;;;;;;;;;;;
; libere la table 
; des jeux
; utilise V0, V1, V2
;;;;;;;;;;;;;;;;;;;;;
free_jtable:
  LD V2, STRUC_SIZE
  LD I, JTABLE
  LD V0, 0
  LD V1, 0
  LD [I], V0
  ADD I, V2
  ADD V1, 1
  SE V1, 8
  JP .-4
  RET

;;;;;;;;;;;;;;;;;;;;;;;;;
; deplace les bijoux
; vrifie pour collision
;;;;;;;;;;;;;;;;;;;;;;;;
move_jewel:
  LD V6, 0 ;index
  JP mj_get_record
mj_next:    
  ADD V6,1
  SNE V6, 8
  RET   
mj_get_record:  
  LD V0, V6
  CALL set_jewel_ptr
  LD V5, [I]
  SE V0, 1
  JP mj_next
mj_move_it: 
  LD V0, V1
  CALL set_sprite_ptr
;efface  
  DRW V2,V3, JEWEL_ROW
;nouvelle position
  ADD V2, V4
  ADD V3, V5
  LD V0, 1
;  LD I,REG_SAVE
;  LD [I], V5
;dessine bijou
  DRW V2, V3, JEWEL_ROW
;  LD I, REG_SAVE
;  LD V5, [I]
  LD V7, 1
  SNE VF, 1 ; collision?
  CALL collision
  LD V0, V6
  CALL set_jewel_ptr
  LD V0, V7
  LD [I], V5
  SE V7, 0
  JP mj_next
  LD V0, V1
  CALL set_sprite_ptr
  DRW V2,V3,JEWEL_ROW
  ADD COUNT, -1
  JP mj_next
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  une collision a eu lieu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
collision:  
  SE V6, ANGLE
  JP loose_points
win_points: 
  LD V8, 4
  LD ST, V8
  SNE V1, PERL
  ADD LLEN, P_VAL
  SNE V1, RUBY
  ADD LLEN, R_VAL
  SNE V1, DIAMOND
  ADD LLEN, D_VAL
  SNE V1, BIG_DIAMOND
  ADD LLEN, BD_VAL
  SE VF, 0
  LD LLEN, 255
  JP coll_exit
loose_points:
  LD V0, 40
  NOISE V0
  SNE V1, PERL
  ADD LLEN, -P_VAL
  SNE V1, RUBY
  ADD LLEN, -R_VAL
  SNE V1, DIAMOND
  ADD LLEN, -D_VAL
  SNE V1, BIG_DIAMOND
  ADD LLEN, -BD_VAL
  SE  VF, 1
  LD LLEN, 0
coll_exit:
  CALL update_line
  LD V7, 0
  RET    
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ajuste I sur le bon sprite
; argument: V0=index sprite
; modifie: V0, I
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
set_sprite_ptr:
  PUSH V1
  LD I, JEWEL0
  LD V1, JEWEL_ROW
; bytes/sprite 4*JEWEL_ROW
  SHL V1
  SHL V1
  SNE V0, 0
  JP set_sp1f
  ADD I, V1
  add V0, -1
  SE V0, 0
  JP .-3
set_sp1f:   
  POP V1   
  RET   
;;;;;;;;;;;;;;;;;;;;
; ajuste I sur l'entree
; de la table JTABLE
; argument: V0 = index
; modifie: V0, I
;;;;;;;;;;;;;;;;;;;;;;
set_jewel_ptr:
  PUSH V1
  LD I, JTABLE
  LD V1, STRUC_SIZE
  SNE V0, 0
  JP sjp_1f
  ADD I, V1
  ADD V0, -1
  SE  V0, 0
  JP .-3
sjp_1f:
  POP V1
  RET

;;;;;;;;;;;;;;;;;
; incremente la variable
; CYCLEH
; utilise: V0, V1,I
;;;;;;;;;;;;;;;;;;;;   
inc_maxjewel:
  LD V0, 8
  SUB V0, MAXJWL
  SE V0, 0
  ADD MAXJWL, 1
  RET
    
MSG_OVER:
  ASCII "Partie terminee!"
    
PACM0:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$0F,$FF,$FF,$FF,$00,$00
  DB $00,$00,$F0,$00,$00,$00,$F0,$00
  DB $00,$0F,$00,$00,$F0,$00,$0F,$00
  DB $00,$F0,$00,$0F,$0F,$00,$00,$F0
  DB $0F,$00,$00,$00,$F0,$00,$0F,$00
  DB $0F,$00,$00,$00,$00,$00,$F0,$00
  DB $0F,$00,$00,$00,$00,$0F,$00,$00
  DB $0F,$00,$00,$00,$00,$0F,$00,$00
  DB $0F,$00,$00,$00,$00,$00,$F0,$00
  DB $0F,$00,$00,$00,$00,$00,$0F,$00
  DB $00,$F0,$00,$00,$00,$00,$00,$F0
  DB $00,$0F,$00,$00,$00,$00,$0F,$00
  DB $00,$00,$F0,$00,$00,$00,$F0,$00
  DB $00,$00,$0F,$FF,$FF,$FF,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00

 PACM45:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$0F,$FF,$FF,$00,$00,$00
  DB $00,$00,$F0,$00,$0F,$00,$00,$00
  DB $00,$0F,$00,$00,$0F,$00,$00,$00
  DB $00,$F0,$00,$00,$0F,$00,$00,$00
  DB $0F,$00,$0F,$00,$0F,$00,$00,$00
  DB $F0,$00,$F0,$F0,$0F,$FF,$FF,$F0
  DB $F0,$00,$0F,$00,$00,$00,$00,$F0
  DB $F0,$00,$00,$00,$00,$00,$00,$F0
  DB $F0,$00,$00,$00,$00,$00,$00,$F0
  DB $F0,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$0F,$00
  DB $00,$F0,$00,$00,$00,$00,$F0,$00
  DB $00,$0F,$00,$00,$00,$0F,$00,$00
  DB $00,$00,$FF,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00
    
PACM90:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$0F,$0F,$00,$00,$F0,$F0,$00
  DB $00,$F0,$00,$F0,$0F,$00,$0F,$00
  DB $0F,$00,$00,$0F,$F0,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$0F,$00,$00,$00,$00,$F0
  DB $0F,$00,$F0,$F0,$00,$00,$00,$F0
  DB $0F,$00,$0F,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $00,$F0,$00,$00,$00,$00,$0F,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$00,$0F,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00

PACM135:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$00,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$F0,$00,$0F,$00,$00
  DB $00,$00,$00,$F0,$00,$00,$F0,$00
  DB $00,$00,$00,$F0,$00,$00,$0F,$00
  DB $00,$00,$00,$F0,$00,$F0,$00,$F0
  DB $0F,$FF,$FF,$F0,$0F,$0F,$00,$F0
  DB $0F,$00,$00,$00,$00,$F0,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $00,$F0,$00,$00,$00,$00,$0F,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$00,$0F,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00
    
PACM180:        
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$FF,$FF,$FF,$FF,$00,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$F0,$00,$0F,$00,$00,$0F,$00
  DB $0F,$00,$00,$F0,$F0,$00,$00,$F0
  DB $00,$F0,$00,$0F,$00,$00,$00,$F0
  DB $00,$0F,$00,$00,$00,$00,$00,$F0
  DB $00,$00,$F0,$00,$00,$00,$00,$F0
  DB $00,$00,$F0,$00,$00,$00,$00,$F0
  DB $00,$0F,$00,$00,$00,$00,$00,$F0
  DB $00,$F0,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$0F,$00
  DB $00,$F0,$00,$00,$00,$00,$F0,$00
  DB $00,$0F,$FF,$FF,$FF,$FF,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00

PACM245:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$FF,$FF,$FF,$FF,$00,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$F0,$00,$00,$00,$00,$0F,$00
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $F0,$00,$00,$0F,$00,$00,$00,$0F
  DB $F0,$00,$00,$F0,$F0,$00,$00,$0F
  DB $F0,$00,$00,$0F,$00,$00,$00,$0F
  DB $F0,$00,$00,$00,$00,$00,$00,$0F
  DB $FF,$FF,$FF,$F0,$00,$00,$00,$0F
  DB $00,$00,$00,$F0,$00,$00,$00,$F0
  DB $00,$00,$00,$F0,$00,$00,$0F,$00
  DB $00,$00,$00,$F0,$00,$00,$F0,$00
  DB $00,$00,$00,$F0,$00,$0F,$00,$00
  DB $00,$00,$00,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00
    
PACM270:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$0F,$FF,$FF,$F0,$00,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$F0,$00,$00,$00,$00,$0F,$00
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$F0,$00,$F0
  DB $0F,$00,$00,$00,$0F,$0F,$00,$F0
  DB $0F,$00,$00,$00,$00,$F0,$00,$F0
  DB $0F,$00,$00,$0F,$F0,$00,$00,$F0
  DB $00,$F0,$00,$F0,$0F,$00,$0F,$00
  DB $00,$0F,$0F,$00,$00,$F0,$F0,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00

PACM315:
  DB $00,$00,$00,$00,$00,$00,$00,$00
  DB $00,$00,$0F,$FF,$FF,$F0,$00,$00
  DB $00,$00,$F0,$00,$00,$0F,$00,$00
  DB $00,$0F,$00,$00,$00,$00,$F0,$00
  DB $00,$F0,$00,$00,$00,$F0,$0F,$00
  DB $0F,$00,$00,$00,$0F,$0F,$00,$F0
  DB $0F,$00,$00,$00,$00,$F0,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$00,$00,$F0
  DB $0F,$00,$00,$00,$00,$FF,$FF,$F0
  DB $0F,$00,$00,$00,$00,$F0,$00,$00
  DB $00,$F0,$00,$00,$00,$F0,$00,$00
  DB $00,$0F,$00,$00,$00,$F0,$00,$00
  DB $00,$00,$F0,$00,$00,$F0,$00,$00
  DB $00,$00,$0F,$FF,$FF,$F0,$00,$00
  DB $00,$00,$00,$00,$00,$00,$00,$00
    
JEWEL0: ; perle 1 points
  DB $00,$00,$00,$00, $00,$00,$00,$00
  DB $00,$0F,$F0,$00, $00,$FF,$FF,$00  
  DB $00,$FF,$FF,$00, $00,$0F,$F0,$00  
  DB $00,$00,$00,$00, $00,$00,$00,$00
    
JEWEL1: ; rubis 5 point 
  DB $00,$00,$00,$00, $00,$FF,$FF,$00
  DB $0F,$F0,$0F,$F0, $FF,$00,$00,$FF  
  DB $0F,$F0,$0F,$F0, $00,$FF,$FF,$00  
  DB $00,$00,$00,$00, $00,$00,$00,$00
    
JEWEL2: ; petit diamant 10 points
  DB $00,$0F,$F0,$00, $00,$F0,$FF,$00
  DB $0F,$00,$0F,$F0, $F0,$00,$00,$FF  
  DB $F0,$00,$00,$FF, $0F,$00,$0F,$F0  
  DB $00,$F0,$FF,$00, $00,$0F,$F0,$00  

JEWEL3: ; gros diamant 20 points
  DB $18, $24
  DB $66, $99
  DB $99, $66
  DB $24, $18
    
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; chaque element de la table
; est une direction
; les donnees d'une direction sont
; 1 libre {0,1}  
; 2 type de bijou {0,1,2,3} 
; 3 x  {0..120}
; 4 y  {0..56}
; 5 dx {-2,0,2}  
; 6 dy {-1,0,1}  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
JTABLE: ; tableau des bijoux
  DW 0,0,240,81,-2,0 ; est
  DW 0,0,240,0,-2,1 ; nord-est
  DW 0,0,116,0,0,1 ; nord
  DW 0,0,0,0,2,1 ; nord-ouest
  DW 0,0,0,81,2,0 ; ouest
  DW 0,0,0,162,2,-1 ; sud-ouest
  DW 0,0,116,162,0,-1 ; sud
  DW 0,0,240,162,-2,-1 ; sud-est

REG_SAVE:
  DW 0,0,0,0,0,0,0,0
  DW 0,0,0,0,0,0,0,8
  
END
    
