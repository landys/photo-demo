package com.netease.space.antispam.picmatcher;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * The implementation of {@link PicMatcher}
 * 
 * @author tony
 */
public class PicMatcherImpl implements PicMatcher {

    private String indexName;

    private String setupIndexTempFile;

    private String setupIndexKeypointsFile;

    private String queryKeypointsFile;

    private String outputFile;
    
    private String curDir;

    private final int imgDbl = 1;

    private final double contrThr = 0.0;

    private final int matchedPointLimit = 10;

    private native int showSift(String imagenamefile, String out_file_name,
            int img_dbl, double contr_thr);

    private native int siftImage(String imagename, String out_file_name,
            int img_dbl, double contr_thr);

    private native void setUpIndex(String dataFile, String index, double R,
            double W, int K, int L);

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
            for (File file : files) {
                pw.write(file.getAbsolutePath() + "\n");
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
    public boolean query(final String fileName) {
        if (fileName == null || fileName.trim().length() == 0) {
            System.out.println("The argument path is empty string.");
            return false;
        }

        File queryImage = new File(fileName);
        File index = new File(indexName);
        File output = new File(outputFile);
        File queryKeypoints = new File(queryKeypointsFile);

        siftImage(queryImage.getAbsolutePath(), queryKeypoints
                .getAbsolutePath(), imgDbl, contrThr);
        query(queryKeypoints.getAbsolutePath(), index.getAbsolutePath(), output
                .getAbsolutePath());

        boolean re = false;
        BufferedReader in = null;
        try {
            in = new BufferedReader(new FileReader(output));
            final String line = in.readLine();
            if (line != null) {
                final String[] splits = line.split("\\*");
                if (splits.length >= 2) {
                    int matchedPoint = 0;
                    try {
                        matchedPoint = Integer.parseInt(splits[1]);
                    } catch (Exception e) {
                        // do nothing.
                    }
                    if (matchedPoint >= matchedPointLimit) {
                        re = true;
                    }
                }
            }
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
     * Demo.
     * 
     * @param args
     */
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out
                    .println("It should contans two arguments. 1-setupIndex, 2-match. i.e.");
            System.out
                    .println(">java -jar PicMatcher.jar 1 \"E:\\testpics\\heads\"");
            System.out
                    .println(">java -jar PicMatcher.jar 2 \"E:\\testpics\\heads\\1.jpg\"");
            return;
        }
        PicMatcherImpl matcher = new PicMatcherImpl("./data");
        if ("1".equals(args[0].trim())) {
            // set up index
            matcher.setupIndex(args[1]);
        } else {
            // query
            System.out.println(matcher.query(args[1]));
        }
    }

}
