import java.util.Map;
import java.util.HashMap;

public class Board {

    private final static int SIZE = 10;
    private final Map<String, Cell> board;

    public Board() {
        board = new HashMap<String, Cell>();
        for (int x = 0; x < Board.SIZE; x++) {
            for (int y = 0; y < Board.SIZE; y++) {
                board.put(Board.coord(x, y), new Cell());
            }
        }
    }

    public void placeShip(Ship ship, String coord, boolean direction) throws InvalidPlacementException, InvalidShipTypeException, InvalidPositionException {
        if (ship == null) throw new InvalidShipTypeException();
        if (!board.containsKey(coord)) throw new InvalidPositionException();

        for (int offset = 0; offset < ship.getLength(); offset++) { // check the ship isn't out of bounds or intersecting
            String nextPosition = getPosPlus(coord, offset, direction);
            if (!board.containsKey(nextPosition) || board.get(nextPosition).isOccupied()) throw new InvalidPlacementException();
        }

        // place the ship
        for (int offset = 0; offset < ship.getLength(); offset++) {
            board.get(getPosPlus(coord, offset, direction)).placeSegment(ship.getSegment(offset));
        }
    }

    private String getPosPlus(String coord, int offset, boolean direction) {
        String[] result = coord.replace('(',' ').replace(')',' ').split(",");
        int x = Integer.parseInt(result[0].trim());
        int y = Integer.parseInt(result[1].trim());
        
        if (direction) {
            return Board.coord(x+offset,y);
        }
        return Board.coord(x, y+offset);
    }

    public void attack(String coord) throws InvalidPositionException {
        if (board.containsKey(coord))
            board.get(coord).attack();
        else throw new InvalidPositionException();
    }

    public boolean hasBeenHit(String coord) throws InvalidPositionException {
        if (board.containsKey(coord))
            return board.get(coord).hasBeenHit();
        else throw new InvalidPositionException();
    }

    @Override // displays the users progression thru game
    public String toString() {
        
        String grid = "";
        for (int x = 0; x < Board.SIZE; x++) {
            grid += "[";
            for (int y = 0; y < Board.SIZE; y++) {
                grid += board.get(coord(x, y)) + " ";
            }
            grid +="]\n";
        }
        return grid;
    }

    // displays whole grid (not hit/miss data)
    public String displaySetup() {
        String grid = "  1 2 3 4 5 6 7 8 9 10\n";
        for (char i = 'A'; i <= 'J'; i++) {
            grid += i + " ";
            for (int j = 1; j < Board.SIZE; j++) {
                grid += board.get(i + "" + j).displaySetup() + " ";
            }
            grid += board.get(i + "" + 10).displaySetup() + "\n";
        }
        return grid;
    }

    public static String coord(int x, int y){
        return "("+x+", "+y+")";
    }
}
