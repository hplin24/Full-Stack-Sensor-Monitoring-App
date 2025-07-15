import express, { Request, Response } from "express";
import router from "./api.route";
import { ApiResponse } from "../../types/api-response";

const app = express();
const port = process.env.PORT || 3000;

app.use("/api", router);

app.get("/", (req: Request, res: Response<ApiResponse>) => {
  res.status(200).json({
    success: true,
    err: 0,
    message: "[/] you are in the base url",
  });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
