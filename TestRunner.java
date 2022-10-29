import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.Ignore;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import static org.junit.Assert.assertEquals;

public class TestRunner {
    public static final String TEST_DB_NAME = "test.db";
    public static final int MAX_USERNAME = 21 - 1; // leave space for \0
    public static final int MAX_EMAIL = 256 - 1; // leave space for \0
    public static final int MAX_ID = 32767; // 2^15 - 1
    public static final int TABLE_MAX_ROWS = 1400;

    public static Process process;
    public static ProcessBuilder builder;
    public static OutputStream stdin;
    public static InputStream stdout;
    public static BufferedReader reader;
    public static BufferedWriter writer;
    public static List<String> reality;

    @After
    public void clearDb() {
        try {
            Process p1 = Runtime.getRuntime().exec("rm -f " + TEST_DB_NAME);
            Process p2 = Runtime.getRuntime().exec("touch " + TEST_DB_NAME);
            p1.waitFor();
            p2.waitFor();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    @Before
    public void initialize() {
        builder = new ProcessBuilder("./main", TEST_DB_NAME);
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
    public void rowsDoPersist() {
        System.out.println(printColor("> Data persists on disk", "yellow"));
        List<String> commands = List.of("insert 1 name mail", ".exit");
        reality = executeCmds(commands);
        initialize();
        commands = List.of("select", ".exit");
        List<String> expectations = List.of("mySQLITE>Returnedrows:", "1namemail", "mySQLITE>");
        reality = executeCmds(commands);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }

    // @Test
    // public void handlesFullTable() {
    //     System.out.println(printColor("> Can't insert into a full table", "yellow"));
    //     List<String> commands = new ArrayList<>(TABLE_MAX_ROWS + 10);
    //     List<String> expectations = new ArrayList<>(TABLE_MAX_ROWS + 10);
    //     reader = new BufferedReader(new InputStreamReader(stdout), 100 * (TABLE_MAX_ROWS + 1));
    //     writer = new BufferedWriter(new OutputStreamWriter(stdin), 100 * (TABLE_MAX_ROWS + 1));
    //     for (int i = 1; i <= TABLE_MAX_ROWS + 1; i++) {
    //         commands.add("insert " + i + " name mail");
    //         expectations.add("mySQLITE>Rowinserted.Currentrows:" + i);
    //     }
    //     commands.add(".exit");
    //     expectations.remove(expectations.size() - 1);
    //     expectations.add("mySQLITE>Tableisfull..");
    //     expectations.add("mySQLITE>");
    //     executeCmds(commands);
    //     assertEquals(expectations, reality);
    //     System.out.println(" " + printColor("> Success!", "green") + "\n");
    // }

    @Test
    public void canInsertAndRetrieveRow() {
        System.out.println(printColor("> Can insert and retrieve a row", "yellow"));
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
        reality = executeCmds(commands);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void acceptsMaxSizeInput() {
        System.out.println(printColor("> Max-sized inputs get accepted", "yellow"));
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
        reality = executeCmds(commands);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void givesErrorOnLongInput() {
        System.out.println(printColor("> Too-big inputs get rejected", "yellow"));
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
        reality = executeCmds(commands);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }
    @Test
    public void givesErrorOnNegativeId() {
        System.out.println(printColor("> Negative ID's get rejected", "yellow"));
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
        reality = executeCmds(commands);
        assertEquals(expectations, reality);
        System.out.println(" " + printColor("> Success!", "green") + "\n");
    }

    public static List<String> executeCmds(List<String> commands) {
        List<String> output = new ArrayList<>();
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
                output.add(line.replaceAll("[|\\s]", ""));
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        return output;
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
