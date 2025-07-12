import express, { Request, Response } from "express";
import router from "./tasks";
const app = express();
const port = process.env.PORT || 3000;

app.use('/api', router);

app.get('/', (req: Request, res: Response) => {
    res.send('Hello');
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});

