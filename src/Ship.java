public class Ship {
    // Where it starts
    // how long it is
    // which direction
    private final int startPosX;
    private final int startPosY;
    private final int length;
    private final boolean direction; // true = horizontal (->) false = vertical (V)
    

    private Ship(int startPosX, int startPosY, int length, boolean direction) {
        this.startPosX=startPosX;
        this.startPosY=startPosY;
        this.length=length;
        this.direction=direction;
    }

    // direction (horiz = true, vert = false)
    public static Ship createShip(int startPosX, int startPosY, int shipLength,
                                    boolean direction, int boardLength) throws InvalidPositionException{
        
        if(0>startPosX || startPosX>=boardLength || startPosY<0 || startPosY>=boardLength){
            throw new InvalidPositionException("Out of bounds");
        }
        else if(direction && !((boardLength-shipLength)>=startPosX)){
            throw new InvalidPositionException("Bad start X");
        }
        else if(!direction && !((boardLength-shipLength)>=startPosY)){
            throw new InvalidPositionException("Bad start Y");
        }

        return new Ship(startPosX, startPosY, shipLength, direction);
    }



}