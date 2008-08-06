package com.picMatcher;

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

    private final String indexName = "pic_index";

    private final String setupIndexTempFile = "index_img_files";

    private final String setupIndexKeypointsFile = "index_keypoints";

    private final String queryKeypointsFile = "query_keypoints";

    private final String outputFile = "output_image";

    private final int imgDbl = 0;

    private final double contrThr = 0.0;

    private final int matchedPointLimit = 10;

    private native int showSift(String imagenamefile, String out_file_name,
            int img_dbl, double contr_thr);

    private native int siftImage(String imagename, String out_file_name,
            int img_dbl, double contr_thr);

    private native void setUpIndex(String dataFile, String index, double R,
            double W, int K, int L);

    private native void query(String queryFile, String index, String output);

    static {
        System.loadLibrary("PicMatcherService");
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
     * Demo.
     * 
     * @param args
     */
    public static void main(String[] args) {
        PicMatcherImpl matcher = new PicMatcherImpl();
        // set up index
        matcher.setupIndex("E:\\testpics\\heads");

        // query
        System.out.println(matcher
                .query("E:\\testpics\\mm270k\\AGRICLT2\\AH267.jpg"));
        System.out.println(matcher
                .query("E:\\testpics\\heads\\199565758487940524.jpg"));
    }

}
