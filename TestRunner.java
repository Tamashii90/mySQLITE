import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import static org.junit.Assert.assertEquals;

public class TestRunner {
    public static final int MAX_USERNAME = 21 - 1; // leave space for \0
    public static final int MAX_EMAIL = 256 - 1; // leave space for \0
    public static final int MAX_ID = 32767; // 2^15 - 1

    public static Process process;
    public static ProcessBuilder builder;
    public static OutputStream stdin;
    public static InputStream stdout;
    public static BufferedReader reader;
    public static BufferedWriter writer;
    public static List<String> reality;


    @Before
    public void initialize() {
        builder = new ProcessBuilder("main.exe");
        builder.redirectErrorStream(true);
        try {
            process = builder.start();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        stdin = process.getOutputStream();
        stdout = process.getInputStream();
        reader = new BufferedReader(new InputStreamReader(stdout));
        writer = new BufferedWriter(new OutputStreamWriter(stdin));
        reality = new ArrayList<>();
    }

    @Test
    public void canInsertAndRetrieveRow() {
        System.out.println(printColor("> Testing whether the user can insert and retrieve a row", "yellow"));
        List<String> commands = List.of(
            "insert 1 name mail",
            "select",
            ".exit"
        );
        List<String> expectations = List.of(
                "mySQLITE>Rowinserted.Currentrows:1",
                "mySQLITE>Returnedrows:",
                "1namemail",
                "mySQLITE>"
        );
        populateReality(commands, expectations);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void acceptsMaxSizeInput() {
        System.out.println(printColor("> Testing whether max-sized inputs get accepted", "yellow"));
        String longName = "a".repeat(MAX_USERNAME);
        String longEmail = "m".repeat(MAX_EMAIL);
        List<String> commands = List.of(
            String.format("insert %d %s %s", MAX_ID, longName, longEmail),
            "select",
            ".exit"
        );
        List<String> expectations = List.of(
                "mySQLITE>Rowinserted.Currentrows:1",
                "mySQLITE>Returnedrows:",
                (MAX_ID + longName + longEmail),
                "mySQLITE>"
        );
        populateReality(commands, expectations);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void givesErrorOnLongInput() {
        System.out.println(printColor("> Testing whether too-big inputs get rejected", "yellow"));
        String longName = "a".repeat(MAX_USERNAME);
        String longEmail = "m".repeat(MAX_EMAIL);
        List<String> commands = List.of(
            String.format("insert %d name mail", MAX_ID + 1),
            "select",
            String.format("insert 1 %s mail", longName + "a"),
            "select",
            String.format("insert 1 name %s", longEmail + "a"),
            "select",
            String.format("insert %d %s %s", MAX_ID + 1, longName + "a", longEmail + "a"),
            "select",
            ".exit"
        );
        List<String> expectations = List.of(
                "mySQLITE>Error:idmustbefrom1to32767.",
                "mySQLITE>Returnedrows:",
                "mySQLITE>Invalidinsertsyntax.",
                "mySQLITE>Returnedrows:",
                "mySQLITE>Invalidinsertsyntax.",
                "mySQLITE>Returnedrows:",
                "mySQLITE>Invalidinsertsyntax.",
                "mySQLITE>Returnedrows:",
                "mySQLITE>"
        );
        populateReality(commands, expectations);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void givesErrorOnNegativeId() {
        System.out.println(printColor("> Testing whether negative ID's get rejected", "yellow"));
        List<String> commands = List.of(
            "insert -1 name mail",
            "select",
            ".exit"
        );
        List<String> expectations = List.of(
                "mySQLITE>Error:idmustbefrom1to32767.",
                "mySQLITE>Returnedrows:",
                "mySQLITE>"
        );
        populateReality(commands, expectations);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }


    public static void populateReality(List<String> commands, List<String> expectations) {
        try {
            for (String cmd : commands) {
                // System.out.println(String.format("    Executing '%s'", cmd));
                writer.write(cmd);
                writer.newLine();
            }
            writer.close();
            while (true) {
                String line = reader.readLine();
                if (line == null) {
                    break;
                }
                reality.add(line.replaceAll("[|\\s]", ""));
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    private static String printColor(String str, String color) {
        Map<String, Integer> colors = Map.of(
            "green", 42,
            "yellow", 43
        );
        // e.g. \u001b[30;43mThis is green BG with black FG\u001b[0m
        return String.format("\u001B[30;%dm%s\u001B[0m", colors.get(color), str);
    }
}
