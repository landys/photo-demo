package com.netease.space.antispam.picmatcher;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.ByteOrder;

/**
 * The implementation of {@link PicMatcher}
 * 
 * @author tony
 */
public class PicMatcherImpl implements PicMatcher {

    private String indexName;

    private String setupIndexTempFile;

    private String setupIndexKeypointsFile;

    private String addToIndexKeypointsFile;

    private String queryKeypointsFile;

    private String outputFile;

    private String curDir;

    private final int imgDbl = 1;

    private final double contrThr = 0.04;

    private final int matchedPointLimit = 80;

    private native int showSift(String imagenamefile, String out_file_name,
            int img_dbl, double contr_thr);

    private native int siftImage(String imagename, String out_file_name,
            int img_dbl, double contr_thr, long id);

    private native void setUpIndex(String dataFile, String index, double R,
            double W, int K, int L);

    private native void addToIndex(String dataFile, String index);

    private native void query(String queryFile, String index, String output);

    private static final String DEFAULT_CURDIR = "./";

    static {
        System.loadLibrary("PicMatcherService");
    }

    public PicMatcherImpl() {
        this(DEFAULT_CURDIR);
    }

    /**
     * @param curDir
     *            current directory for temp files.
     */
    public PicMatcherImpl(String curDir) {
        setCurDir(curDir);

        indexName = this.curDir + "pic_index";

        setupIndexTempFile = this.curDir + "index_img_files";

        setupIndexKeypointsFile = this.curDir + "index_keypoints";

        addToIndexKeypointsFile = this.curDir + "add_keypoints";

        queryKeypointsFile = this.curDir + "query_keypoints";

        outputFile = this.curDir + "output_image";
    }

    /**
     * {@inheritDoc}
     */
    public void setupIndex(final String path) {
        if (path == null || path.trim().length() == 0) {
            System.out.println("The argument path is empty string.");
            return;
        }

        File dir = new File(path.trim());
        if (!dir.isDirectory()) {
            System.out.println("The path is not a directory: " + path);
            return;
        }

        File[] files = dir.listFiles(new FilenameFilter() {
            /**
             * {@inheritDoc}
             */
            public boolean accept(File dir, String name) {
                final String fileName = name.toLowerCase();
                if (fileName.endsWith("jpg") || fileName.endsWith("jpeg")) {
                    return true;
                }
                return false;
            }

        });

        PrintWriter pw = null;
        File temp = new File(setupIndexTempFile);
        try {
            pw = new PrintWriter(temp);
            int i = 0;
            for (File file : files) {
                StringBuilder sb = new StringBuilder();
                sb.append(++i).append(' ').append(file.getAbsolutePath())
                        .append('\n');
                pw.write(sb.toString());
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return;
        } finally {
            if (pw != null) {
                pw.close();
            }
        }

        File index = new File(indexName);
        File output = new File(setupIndexKeypointsFile);

        showSift(temp.getAbsolutePath(), output.getAbsolutePath(), imgDbl,
                contrThr);
        setUpIndex(output.getAbsolutePath(), index.getAbsolutePath(), 0.0, 0.0,
                0, 0);
    }

    /**
     * {@inheritDoc}
     */
    public void addToIndex(final String fileName, final long id) {
        if (fileName == null || fileName.trim().length() == 0) {
            System.out.println("The argument fileName is empty string.");
            return;
        }
        if (id <= 0) {
            System.out.println("The id must not be positive: " + id);
        }

        File queryImage = new File(fileName);
        File index = new File(indexName);
        File addKeypoints = new File(addToIndexKeypointsFile);

        siftImage(queryImage.getAbsolutePath(), addKeypoints.getAbsolutePath(),
                imgDbl, contrThr, id);
        addToIndex(addKeypoints.getAbsolutePath(), index.getAbsolutePath());
    }

    /**
     * {@inheritDoc}
     */
    public long query(final String fileName) {
        if (fileName == null || fileName.trim().length() == 0) {
            System.out.println("The argument fileName is empty string.");
            return -1;
        }

        File queryImage = new File(fileName);
        File index = new File(indexName);
        File output = new File(outputFile);
        File queryKeypoints = new File(queryKeypointsFile);

        siftImage(queryImage.getAbsolutePath(), queryKeypoints
                .getAbsolutePath(), imgDbl, contrThr, 0);
        query(queryKeypoints.getAbsolutePath(), index.getAbsolutePath(), output
                .getAbsolutePath());

        long re = -1;
        DataInputStream in = null;
        try {
            in = new DataInputStream(new FileInputStream(output));

            long id = readLongNativeEndian(in);
            int matchedPoints = readIntNativeEndian(in);
            if (!(id == -1 && matchedPoints == -1)
                    && matchedPoints >= matchedPointLimit) {
                re = id;
            }
        } catch (EOFException e) {
            // end of file reached, it is ok, and nothing will be done here.
            System.out.println("End of file reached.");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return re;
    }

    /**
     * @return the curDir.
     */
    public String getCurDir() {
        return curDir;
    }

    /**
     * @param curDir
     *            the curDir to set.
     */
    public void setCurDir(final String curDir) {
        boolean goodDir = false;
        String dir = null;
        if (curDir != null) {
            dir = curDir.replaceAll("\\\\", "/");
            final File file = new File(dir);
            if (!file.isDirectory()) {
                if (!file.exists()) {
                    if (file.mkdirs()) {
                        goodDir = true;
                    } else {
                        System.out.println("ERROR: Create directory error: "
                                + dir);
                    }
                } else {
                    System.out
                            .println("ERROR: The path exists, but not a directory: "
                                    + dir);
                }
            } else {
                goodDir = true;
            }
        }

        if (goodDir) {
            this.curDir = dir;
            if (!this.curDir.endsWith("/")) {
                this.curDir += '/';
            }
        } else {
            this.curDir = DEFAULT_CURDIR;
        }
    }

    /**
     * Read the next eight bytes of this input stream, interpreted as a long
     * according to native byte endian. If the native endian is big endian, use
     * the java way; it it's little endian, interpret according to little endian
     * rule.
     * 
     * @param in
     *            the input stream.
     * @return the next eight bytes of this input stream, interpreted as a long.
     * @throws EOFException
     *             if this input stream reaches the end before reading eight
     *             bytes.
     * @throws IOException
     *             the stream has been closed and the contained input stream
     *             does not support reading after close, or another I/O error
     *             occurs.
     */
    private long readLongNativeEndian(final DataInputStream in)
            throws IOException {
        if (ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN) {
            return in.readLong();
        }
        // 8 bytes
        long accum = 0;
        for (int shiftBy = 0; shiftBy < 64; shiftBy += 8) {
            // must cast to long or shift done modulo 32
            accum |= (long) (in.readByte() & 0xff) << shiftBy;
        }

        return accum;
    }

    /**
     * Read the next eight bytes of this input stream, interpreted as an integer
     * according to native byte endian. If the native endian is big endian, use
     * the java way; it it's little endian, interpret according to little endian
     * rule.
     * 
     * @param in
     *            the input stream.
     * @return the next eight bytes of this input stream, interpreted as an
     *         integer.
     * @throws EOFException
     *             if this input stream reaches the end before reading four
     *             bytes.
     * @throws IOException
     *             the stream has been closed and the contained input stream
     *             does not support reading after close, or another I/O error
     *             occurs.
     */
    private int readIntNativeEndian(final DataInputStream in)
            throws IOException {
        if (ByteOrder.nativeOrder() == ByteOrder.BIG_ENDIAN) {
            return in.readInt();
        }

        // 4 bytes
        int accum = 0;
        for (int shiftBy = 0; shiftBy < 32; shiftBy += 8) {
            accum |= (in.readByte() & 0xff) << shiftBy;
        }
        return accum;
    }

    /**
     * Demo.
     * 
     * @param args
     */
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out
                    .println("It should contans two arguments. 1-setupIndex, 2-addtoIndex, 3-match. i.e.");
            System.out
                    .println(">java -jar PicMatcher.jar 1 \"E:\\testpics\\heads\"");
            System.out
                    .println(">java -jar PicMatcher.jar 2 \"E:\\testpics\\heads\\1.jpg\" 212");
            System.out
                    .println(">java -jar PicMatcher.jar 3 \"E:\\testpics\\heads\\2.jpg\"");
            return;
        }
        PicMatcherImpl matcher = new PicMatcherImpl("./data");
        if ("1".equals(args[0].trim())) {
            // set up index
            matcher.setupIndex(args[1]);
        } else if ("2".equals(args[0].trim())) {
            // add to index
            matcher.addToIndex(args[1], Long.parseLong(args[2]));
        } else {
            // query
            System.out.println(matcher.query(args[1]));
        }
    }

}
